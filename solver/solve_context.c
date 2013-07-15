#include "solve_context.h"

static int _lookup_solver(SolveContext * context, const char * name);
static CLArgumentList * _argument_list(SolveContext * context, int argc, const char * argv[]);

static int _load_solver(SolveContext * context, FILE * fp);
static int _load_search_parameters(SolveContext * context, FILE * fp);
static int _load_search_heuristics(SolveContext * context, FILE * fp);
static void _copy_parameters_from_state(SolveContext * context, CSSearchState * state);

static void _destroy_search_heuristics(SolveContext * context);

/***********
 * Loading *
 ***********/

CSSearchState * sc_load(SolveContext * context, const char * fileName) {
    FILE * fp = fopen(fileName, "r");
    if (!fp) {
        fprintf(stderr, "Error: failed to open file.\n");
        return NULL;
    }
    if (!_load_solver(context, fp)) {
        fprintf(stderr, "Error: failed to load the solver name.\n");
        fclose(fp);
        return NULL;
    }
    if (!_load_search_parameters(context, fp)) {
        fprintf(stderr, "Error: failed to load search parameters.\n");
        fclose(fp);
        return NULL;
    }
    
    CSSearchState * state = load_cuboid_search(fp);
    if (!state) {
        fprintf(stderr, "Error: failed to load search state.\n");
        fclose(fp);
        return NULL;
    }
    
    // we must make sure our CLSearchParameters are accurate before
    // we initialize the solver and heuristic list.
    _copy_parameters_from_state(context, state);
    
    if (!_load_search_heuristics(context, fp)) {
        fprintf(stderr, "Error: failed to load search heuristics.\n");
        fclose(fp);
        return NULL;
    }
    
    int result = context->solver.resume(&context->searchParameters, fp, &context->userData);
    fclose(fp);
    if (!result) {
        cs_search_state_free(state);
        heuristic_list_free(context->searchParameters.heuristics);
        return NULL;
    }
    return state;
}

void sc_save(SolveContext * context, CSSearchState * state, const char * fileName) {
    FILE * fp = fopen(fileName, "w");
    
    save_string(context->solver.name, fp);
    
    uint8_t verboseFlag = context->searchParameters.verboseFlag;
    uint8_t multipleFlag = context->searchParameters.multipleFlag;
    fwrite(&verboseFlag, 1, 1, fp);
    fwrite(&multipleFlag, 1, 1, fp);
    
    save_cuboid_search(state, fp);
    save_heuristic_list(context->searchParameters.heuristics, fp);
    
    context->solver.save(context->userData, fp);
        
    fclose(fp);
    
    printf("Saved to %s.\n", fileName);
}

/**************
 * Initiating *
 **************/

Cuboid * sc_standard_solve_input(SolveContext * context, int argc, const char * argv[]) {
    if (!_lookup_solver(context, argv[1])) {
        fprintf(stderr, "Error: invalid solver `%s`.\n", argv[1]);
        return NULL;
    }
    Solver solver = context->solver;
    
    // parse the general arguments
    CLArgumentList * args = _argument_list(context, argc - 2, &argv[2]);
    if (!args) return NULL;
    
    if (!cl_sa_process(args, &context->searchParameters)) {
        fprintf(stderr, "Error: failed to process search parameters.\n");
        cl_argument_list_free(args);
        return NULL;
    }
    
    // parse the solver-specific arguments
    cl_sa_remove_all(args);
    int result = solver.run(&context->searchParameters, args, &context->userData);
    cl_argument_list_free(args);
    if (!result) {
        sc_release_resources(context);
        return NULL;
    }
    
    // input the cuboid
    Cuboid * cuboid = input_stickermap_to_cuboid(context->searchParameters.dimensions);
    if (!cuboid) {
        solver.completed(context->userData);
        sc_release_resources(context);
        return NULL;
    }
    
    return cuboid;
}

CSSettings sc_generate_cs_settings(SolveContext * context, Cuboid * root) {
    int hasHeuristics = context->searchParameters.heuristics->count > 0;
    
    CSSettings settings;
    settings.cacheCuboid = context->solver.cacheCuboid | hasHeuristics;
    settings.rootNode = root;
    settings.algorithms = context->searchParameters.operations;
    return settings;
}

BSSettings sc_generate_bs_settings(SolveContext * context) {
    BSSettings settings;
    settings.threadCount = context->searchParameters.threadCount;
    settings.minDepth = context->searchParameters.minDepth;
    settings.maxDepth = context->searchParameters.maxDepth;
    settings.nodeInterval = kSearchNodeInterval;
    return settings;
}

/*********************
 * Memory Management *
 *********************/

void sc_release_resources(SolveContext * context) {
    if (context->searchContext) {
        cs_context_release(context->searchContext);
    } else {
        alg_list_release(context->searchParameters.operations);
    }
    heuristic_list_free(context->searchParameters.heuristics);
}

/***********
 * Private *
 ***********/

static int _lookup_solver(SolveContext * context, const char * name) {
    int i;
    for (i = 0; i < SolverTableCount; i++) {
        if (strcmp(name, SolverTable[i].name) == 0) {
            context->solver = SolverTable[i];
            return 1;
        }
    }
    return 0;
}

static CLArgumentList * _argument_list(SolveContext * context, int argc, const char * argv[]) {
    CLArgumentList * solverArgs = context->solver.default_arguments();
    CLArgumentList * normalDefaults = cl_sa_default_arguments(argc, argv);
    cl_argument_list_add_all(normalDefaults, solverArgs);
    cl_argument_list_free(solverArgs);
    
    int failIndex = 0;
    CLArgumentList * result = cl_parse_arguments(argv, argc, normalDefaults, &failIndex);
    cl_argument_list_free(normalDefaults);
    if (!result) {
        fprintf(stderr, "Error: invalid argument near `%s`.\n", argv[failIndex]);
        return NULL;
    }
    return result;
}

/*******************
 * Private Loading *
 *******************/

static int _load_solver(SolveContext * context, FILE * fp) {
    char * nameBuffer = load_string(fp);
    int res = _lookup_solver(context, nameBuffer);
    free(nameBuffer);
    return res;
}

static int _load_search_parameters(SolveContext * context, FILE * fp) {
    uint8_t verboseFlag, multipleFlag;
    if (fread(&verboseFlag, 1, 1, fp) != 1) return 0;
    if (fread(&multipleFlag, 1, 1, fp) != 1) return 0;
    context->searchParameters.verboseFlag = verboseFlag;
    context->searchParameters.multipleFlag = multipleFlag;
    return 1;
}

static int _load_search_heuristics(SolveContext * context, FILE * fp) {
    CuboidDimensions dims = context->searchParameters.dimensions;
    HeuristicList * list = load_heuristic_list(fp, dims);
    context->searchParameters.heuristics = list;
    
    if (!list) return 0;
    
    Cuboid * temp = cuboid_create(dims);
    heuristic_list_prepare(list, temp);
    cuboid_free(temp);
    
    return 1;
}

static void _copy_parameters_from_state(SolveContext * context, CSSearchState * state) {
    context->searchParameters.minDepth = state->bsState->settings.minDepth;
    context->searchParameters.maxDepth = state->bsState->settings.maxDepth;
    context->searchParameters.threadCount = state->bsState->settings.threadCount;
    context->searchParameters.dimensions = state->settings.rootNode->dimensions;
    context->searchParameters.operations = state->settings.algorithms;
}
