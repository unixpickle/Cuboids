#ifndef __SEARCH_ARGS_H__
#define __SEARCH_ARGS_H__

#include "arguments.h"
#include "algebra/basis.h"

typedef struct {
    int minDepth, maxDepth;
    int threadCount;
    int verboseFlag, multipleFlag;
    CuboidDimensions dimensions;
    AlgList * operations;
} CLSearchParameters;

CLArgumentList * cl_sa_default_arguments();
int cl_sa_process(CLArgumentList * args, CLSearchParameters * params);

#endif