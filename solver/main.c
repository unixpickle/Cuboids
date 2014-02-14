#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "solvers.h"
#include "notation/print.h"
#include "solve_context.h"

#define MANUAL_HEURISTIC_CHECK 1

// magical global variables
static SolveContext solveContext;
static pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;
static int foundSolution = 0;

void handle_interrupt(int dummy);
void print_usage(const char * command);
void print_solver_usage(const char * solver);

int handle_resume(int argc, const char * argv[]);
int handle_solver(int argc, const char * argv[]);

void dispatch_search(Cuboid * root);
void dispatch_resume(CSSearchState * state);

CSCallbacks generate_callbacks();

void search_handle_progress(void * data);
void search_handle_depth(void * data, int depth);
int search_accepts_sequence(void * data, const int * seq, int len, int depthRem);
int search_accepts_cuboid(void * data, const Cuboid * cuboid, Cuboid * cache, int depthRem);
void search_handle_cuboid(void * data, const Cuboid * cuboid, Cuboid * cache,
                          const int * sequence, int len);
void search_handle_save_data(void * data, CSSearchState * save);
void search_handle_finished(void * data);

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    bzero(&solveContext.searchParameters, sizeof(CLSearchParameters));
    bzero(&solveContext.solver, sizeof(Solver));
    
    signal(SIGINT, handle_interrupt);
    
    if (strcmp(argv[1], "resume") == 0) {
        return handle_resume(argc, argv);
    } else if (strcmp(argv[1], "help") == 0) { 
        if (argc != 3) {
            fprintf(stderr, "Error: help takes exactly one argument\n");
            return 1;
        }
        print_solver_usage(argv[2]);
    } else {
        return handle_solver(argc, argv);
    }
}

void handle_interrupt(int dummy) {
    if (solveContext.searchContext) {
        cs_context_stop(solveContext.searchContext, 1);
    } else {
        printf("\n");
        exit(0);
    }
}

void print_usage(const char * command) {
    printf("Usage: %s [<solver> <options> | resume <file.sav> | help solver ]\n", command);
    puts("Options:");
    puts(" --multiple        find multiple solutions");
    puts(" --verbose         display periodic updates");
    puts(" --mindepth=n      the minimum search depth [0]");
    puts(" --maxdepth=n      the maximum search depth [20]");
    puts(" --threads=n       the number of search threads to use [8]");
    puts(" --operations <x>  the , separated operations to use");
    puts(" --dimensions <x>  the dimensions in XxYxZ format. [3x3x3]");
    puts(" --heuristic <x>   a heuristic database to use.");
    puts("\nAvailable solvers:\n");
    int i;
    for (i = 0; i < SolverTableCount; i++) {
        Solver s = SolverTable[i];
        printf("%12s - %s\n", s.name, s.description);
    }
    printf("\n\n");
}

void print_solver_usage(const char * solver) {
    int i, j;
    for (i = 0; i < SolverTableCount; i++) {
        Solver s = SolverTable[i];
        if (strcmp(s.name, solver) != 0) continue;
        
        printf("Usage: solver %s ", solver);
        CLArgumentList * defArgs = s.default_arguments();
        cl_sa_print_usage(defArgs);
        cl_argument_list_free(defArgs);
        printf("\n");
        
        return;
    }
    fprintf(stderr, "Error: solver `%s` does not exist.\n", solver);
}

/*********************************
 * Different kinds of invocation *
 *********************************/

int handle_resume(int argc, const char * argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    CSSearchState * state = sc_load(&solveContext, argv[2]);
    if (!state) {
        return 1;
    }
    
    dispatch_resume(state);
    while (1) sleep(1);
    return 0;
}

int handle_solver(int argc, const char * argv[]) {
    Cuboid * cuboid = sc_standard_solve_input(&solveContext, argc, argv);
    if (!cuboid) {
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
    CSSettings settings = sc_generate_cs_settings(&solveContext, root);
    BSSettings bsSettings = sc_generate_bs_settings(&solveContext);
    solveContext.searchContext = cs_run(settings, bsSettings, callbacks);
}

void dispatch_resume(CSSearchState * state) {
    CSCallbacks callbacks = generate_callbacks();
    solveContext.searchContext = cs_resume(state, callbacks);
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

/*************
 * Callbacks *
 *************/

void search_handle_progress(void * data) {
    if (!solveContext.searchParameters.verboseFlag) return;
    BSProgress progress = bs_context_progress(solveContext.searchContext->bsContext);
    int depth = bs_context_current_depth(solveContext.searchContext->bsContext);
    printf("expanded %llu, pruned %llu, depth %d\n",
           progress.nodesExpanded, progress.nodesPruned, depth);
}

void search_handle_depth(void * data, int depth) {
    printf("Exploring depth of %d...\n", depth);
}

int search_accepts_sequence(void * data, const int * seq, int len, int depthRem) {
    // TODO: insert intelligent pruning here...
    return 1;
}

int search_accepts_cuboid(void * data, const Cuboid * cuboid, Cuboid * cache, int depthRem) {
    CLSearchParameters params = solveContext.searchParameters;
    if (MANUAL_HEURISTIC_CHECK) {
        int i;
        for (i = 0; i < params.heuristics->count; i++) {
            Heuristic * h = params.heuristics->heuristics[i];
            int value = heuristic_pruning_value(h, cuboid, cache);
            if (value > depthRem) return 0;
        }
        return 1;
    } else {
        return !heuristic_list_exceeds(params.heuristics, cuboid, cache, depthRem);
    }
}

void search_handle_cuboid(void * data, const Cuboid * cuboid, Cuboid * cache,
                          const int * sequence, int len) {
    if (solveContext.solver.is_goal(solveContext.userData, cuboid, cache)) {
        pthread_mutex_lock(&printMutex);
        if (foundSolution && !solveContext.searchParameters.multipleFlag) {
            pthread_mutex_unlock(&printMutex);
            return;
        }
        foundSolution = 1;
        printf("Found solution:");
        int i;
        
        // note that this will only be valid while the search context
        // is retained...
        AlgList * list = solveContext.searchContext->settings.algorithms;
        
        for (i = 0; i < len; i++) {
            printf(" ");
            Algorithm * a = list->entries[sequence[i]].algorithm;
            algorithm_print(a, stdout);
        }
        
        printf("\n");
        pthread_mutex_unlock(&printMutex);
        
        if (!solveContext.searchParameters.multipleFlag) {
            cs_context_stop(solveContext.searchContext, 0);
        }
    }
}

void search_handle_save_data(void * data, CSSearchState * save) {
    printf("\nWould you like to save? [Y/n]: ");
    fflush(stdout);
    
    char c = fgetc(stdin);
    if (c == 'y' || c == 'Y') {
        char fileName[512];
        sprintf(fileName, "save_%lld.dat", (long long int)time(NULL));
        sc_save(&solveContext, save, fileName);
    }
    cs_search_state_free(save);
}

void search_handle_finished(void * data) {
    solveContext.solver.completed(solveContext.userData);
    sc_release_resources(&solveContext);
    exit(0);
}
