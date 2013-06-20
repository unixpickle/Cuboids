#ifndef __SOLVE_CONTEXT_H__
#define __SOLVE_CONTEXT_H__

#include <stdio.h>

#include "solvers.h"
#include "arguments/parser.h"
#include "search/cuboid.h"
#include "saving/save_search.h"
#include "input/sticker_input.h"

#define kSearchNodeInterval 500000

typedef struct {
    CLSearchParameters searchParameters;
    void * userData;
    CSSearchContext * searchContext;
    Solver solver;
} SolveContext;

// loading a solve
CSSearchState * sc_load(SolveContext * context, const char * fileName);
void sc_save(SolveContext * context, CSSearchState * state, const char * fileName);

// creating a new search
Cuboid * sc_standard_solve_input(SolveContext * context, int argc, const char * argv[]);
CSSettings sc_generate_cs_settings(SolveContext * context, Cuboid * root);
BSSettings sc_generate_bs_settings(SolveContext * context);

// terminating a search
void sc_release_resources(SolveContext * context);

#endif
