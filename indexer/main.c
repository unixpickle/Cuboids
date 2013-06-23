#include "indexer_arguments.h"
#include "arguments/parser.h"
#include "heuristic/heuristic_io.h"
#include <stdio.h>

static Heuristic * heuristic = NULL;
static IndexerArguments arguments;

void print_usage(const char * name);
CLArgumentList * process_arguments(int argc, const char * argv[]);
HSParameters heuristic_parameters(IndexerArguments args);

int generate_heuristic(const char * name, CLArgumentList * args);
int run_search();

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    IndexerArguments args;
    CLArgumentList * list = process_arguments(argc, argv);
    if (!list) return 1;
    if (!indexer_process_arguments(list, &arguments)) {
        fprintf(stderr, "error: failed to process arguments.\n");
        cl_argument_list_free(list);
        return 1;
    }
    
    indexer_remove_all_defaults(list);
    int result = generate_heuristic(argv[1], list);
    cl_argument_list_free(list);
    if (!result) {
        alg_list_release(arguments.operations);
        fprintf(stderr, "error: failed to create heuristic.\n");
        return 1;
    }
    result = run_search();
    if (!result) {
        fprintf(stderr, "error: failed to launch search.\n");
        alg_list_release(arguments.operations);
        heuristic_free(heuristic);
        return 1;
    }
    return 0;
}

void print_usage(const char * name) {
    fprintf(stderr, "Usage: %s <index type> [--maxdepth=n] [--threads=n]\n\
       [--operations ...] [--symmetries xyz]\n\n", name);
    fflush(stderr);
}

CLArgumentList * process_arguments(int argc, const char * argv[]) {
    CLArgumentList * defaults = indexer_default_arguments();
    int failIndex;
    CLArgumentList * result = cl_parse_arguments(&argv[2], argc - 2, defaults, &failIndex);
    cl_argument_list_free(defaults);
    if (!result) {
        fprintf(stderr, "error: invalid argument near `%s`\n", argv[failIndex + 2]);
    }
    return result;
}

HSParameters heuristic_parameters(IndexerArguments args) {
    HSParameters params;
    params.symmetries = args.symmetries;
    params.maxDepth = args.maxDepth;
    return params;
}

/*********************
 * Commencing action *
 *********************/

int generate_heuristic(const char * name, CLArgumentList * args) {
    HSParameters params = heuristic_parameters(arguments);
    heuristic = heuristic_create(params, args, name);
    return (heuristic != NULL);
}

int run_search() {
    return 0;
}
