#include "solvers.h"
#include "arguments/parser.h"
#include <stdio.h>

static CLSearchParameters searchParameters;
static void * userData;
static CSSearchContext * searchContext;

void print_usage(const char * command);

int handle_resume(int argc, const char * argv[]);
int handle_solver(int argc, const char * argv[]);

int command_lookup_solver(Solver * solver, const char * name);
CLArgumentList * command_argument_list(Solver solver, int argc, const char * argv[]);

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
    Solver solver;
    if (!command_lookup_solver(&solver, argv[1])) {
        fprintf(stderr, "Error: invalid solver `%s`.\n", argv[1]);
        return 1;
    }
    
    CLArgumentList * args = command_argument_list(solver, argc - 2, &argv[2]);
    if (!args) return 1;
    
    if (!cl_sa_process(args, &searchParameters)) {
        fprintf(stderr, "Error: failed to process search parameters.\n");
        cl_argument_list_free(args);
        return 1;
    }
    
    cl_sa_remove_all(args);
    if (!solver.run(&searchParameters, args, &userData)) {
        return 1;
    }
    
    
    
    cl_argument_list_free(args);
    
}

/**********************
 * Command processing *
 **********************/

int command_lookup_solver(Solver * solver, const char * name) {
    int i;
    for (i = 0; i < SolverTableCount; i++) {
        if (strcmp(name, SolverTable[i].name) == 0) {
            *solver = SolverTable[i];
            return 1;
        }
    }
    return 0;
}

CLArgumentList * command_argument_list(Solver solver, int argc, const char * argv[]) {
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
