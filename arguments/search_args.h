#ifndef __SEARCH_ARGS_H__
#define __SEARCH_ARGS_H__

#include "arguments.h"
#include "algebra/basis.h"
#include "heuristic/heuristic_io.h"

typedef struct {
    int minDepth, maxDepth;
    int threadCount;
    int verboseFlag, multipleFlag;
    CuboidDimensions dimensions;
    AlgList * operations;
    HeuristicList * heuristics;
} CLSearchParameters;

CLArgumentList * cl_sa_default_arguments();
int cl_sa_process(CLArgumentList * args, CLSearchParameters * params);
int cl_sa_parse_dimensions(const char * str, CuboidDimensions * dims);

// removes all default command line arguments from the list
void cl_sa_remove_all(CLArgumentList * args);
void cl_sa_print_usage(CLArgumentList * defs);

#endif