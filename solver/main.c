#include "solvers.h"
#include "arguments/parser.h"
#include "input/sticker_input.h"
#include "search/cuboid.h"
#include "notation/print.h"
#include <stdio.h>

#define kSearchNodeInterval 1000000

// magical global variables
static CLSearchParameters searchParameters;
static void * userData;
static CSSearchContext * searchContext;
static Solver solver;
static pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;

void print_usage(const char * command);

int handle_resume(int argc, const char * argv[]);
int handle_solver(int argc, const char * argv[]);

void dispatch_search(Cuboid * root);

CSCallbacks generate_callbacks();
CSSettings generate_cs_settings(Cuboid * root);
BSSettings generate_bs_settings();

int command_lookup_solver(const char * name);
CLArgumentList * command_argument_list(int argc, const char * argv[]);

void search_handle_progress(void * data);
void search_handle_depth(void * data, int depth);
int search_accepts_sequence(void * data, const int * seq, int len, int depthRem);
int search_accepts_cuboid(void * data, Cuboid * cuboid, StickerMap * cache, int depthRem);
void search_handle_cuboid(void * data, Cuboid * cuboid, StickerMap * cache,
                          const int * sequence, int len);
void search_handle_save_data(void * data, CSSearchState * save);
void search_handle_finished(void * data);

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    return handle_solver(argc, argv);
}

void print_usage(const char * command) {
    printf("Usage: %s [<solver> <options> | resume <file.sav>]\n", command);
    puts("Options:");
    puts(" --multiple        find multiple solutions");
    puts(" --verbose         display periodic updates");
    puts(" --mindepth=n      the minimum search depth [0]");
    puts(" --maxdepth=n      the maximum search depth [20]");
    puts(" --threads=n       the number of search threads to use [8]");
    puts(" --operations <x>  the , separated operations to use");
    puts(" --dimensions <x>  the dimensions in XxYxZ format. [3x3x3]");
    printf("\n\n");
}

/*********************************
 * Different kinds of invocation *
 *********************************/

int handle_resume(int argc, const char * argv[]) {
    
}

int handle_solver(int argc, const char * argv[]) {
    if (!command_lookup_solver(argv[1])) {
        fprintf(stderr, "Error: invalid solver `%s`.\n", argv[1]);
        return 1;
    }
    
    // parse the general arguments
    CLArgumentList * args = command_argument_list(argc - 2, &argv[2]);
    if (!args) return 1;
    
    if (!cl_sa_process(args, &searchParameters)) {
        fprintf(stderr, "Error: failed to process search parameters.\n");
        cl_argument_list_free(args);
        return 1;
    }
    
    // parse the solver-specific arguments
    cl_sa_remove_all(args);
    if (!solver.run(&searchParameters, args, &userData)) {
        alg_list_release(searchParameters.operations);
        cl_argument_list_free(args);
        return 1;
    }
    cl_argument_list_free(args);
    
    // input the cuboid
    Cuboid * cuboid = input_stickermap_to_cuboid(searchParameters.dimensions);
    if (!cuboid) {
        solver.completed(userData);
        alg_list_release(searchParameters.operations);
        return 1;
    }
    
    // solve the cubiod!
    dispatch_search(cuboid);
    
    // wait forever on this thread to prevent termination
    while (1) sleep(1);
    return 0;
}

/***************************
 * Calling on the searcher *
 ***************************/

void dispatch_search(Cuboid * root) {
    CSCallbacks callbacks = generate_callbacks();
    CSSettings settings = generate_cs_settings(root);
    BSSettings bsSettings = generate_bs_settings();
    searchContext = cs_run(settings, bsSettings, callbacks);
}

CSCallbacks generate_callbacks() {
    CSCallbacks cbs;
    cbs.userData = NULL;
    cbs.handle_progress = search_handle_progress;
    cbs.handle_depth = search_handle_depth;
    cbs.accepts_sequence = search_accepts_sequence;
    cbs.accepts_cuboid = search_accepts_cuboid;
    cbs.handle_cuboid = search_handle_cuboid;
    cbs.handle_save_data = search_handle_save_data;
    cbs.handle_finished = search_handle_finished;
    return cbs;
}

CSSettings generate_cs_settings(Cuboid * cuboid) {
    CSSettings settings;
    settings.cacheStickerMaps = solver.cacheStickerMaps;
    settings.rootNode = cuboid;
    settings.algorithms = searchParameters.operations;
    return settings;
}

BSSettings generate_bs_settings() {
    BSSettings settings;
    settings.threadCount = searchParameters.threadCount;
    settings.minDepth = searchParameters.minDepth;
    settings.maxDepth = searchParameters.maxDepth;
    settings.nodeInterval = kSearchNodeInterval;
    return settings;
}

/**********************
 * Command processing *
 **********************/

int command_lookup_solver(const char * name) {
    int i;
    for (i = 0; i < SolverTableCount; i++) {
        if (strcmp(name, SolverTable[i].name) == 0) {
            solver = SolverTable[i];
            return 1;
        }
    }
    return 0;
}

CLArgumentList * command_argument_list(int argc, const char * argv[]) {
    CLArgumentList * solverArgs = solver.default_arguments();
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

/*************
 * Callbacks *
 *************/

void search_handle_progress(void * data) {
    if (!searchParameters.verboseFlag) return;
    BSProgress progress = bs_context_progress(searchContext->bsContext);
    int depth = bs_context_current_depth(searchContext->bsContext);
    printf("expanded %lld, pruned %lld, depth %d\n",
           progress.nodesExpanded, progress.nodesPruned, depth);
}

void search_handle_depth(void * data, int depth) {
    printf("Exploring depth of %d...\n", depth);
}

int search_accepts_sequence(void * data, const int * seq, int len, int depthRem) {
    // TODO: insert intelligent pruning here...
    return 1;
}

int search_accepts_cuboid(void * data, Cuboid * cuboid, StickerMap * cache, int depthRem) {
    // TODO: insert heuristic pattern database lookup HERE...
    return 1;
}

void search_handle_cuboid(void * data, Cuboid * cuboid, StickerMap * cache,
                          const int * sequence, int len) {
    if (solver.is_goal(userData, cuboid, cache)) {
        pthread_mutex_lock(&printMutex);
        printf("Found solution:");
        int i;
        
        // note that this will only be valid while the search context
        // is retained...
        AlgList * list = searchParameters.operations;
        
        for (i = 0; i < len; i++) {
            printf(" ");
            Algorithm * a = list->entries[sequence[i]].algorithm;
            algorithm_print(a, stdout);
        }
        
        printf("\n");
        pthread_mutex_unlock(&printMutex);
    }
}

void search_handle_save_data(void * data, CSSearchState * save) {
    printf("Would you like to save? [Y/n]: ");
    fflush(stdout);
    
    char c = fgetc(stdin);
    if (c == 'y' || c == 'Y') {
        // TODO: save entire thing here...
    } else {
        cs_search_state_free(save);
    }
}

void search_handle_finished(void * data) {
    solver.completed(userData);
    cs_context_release(searchContext);
    exit(0);
}
