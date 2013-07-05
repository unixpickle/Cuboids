#ifndef __SOLVERS_H__
#define __SOLVERS_H__

/**
 * This is essentially an interface which all solvers must
 * implement in order to be registered with the main program.
 */

#include <stdio.h>

#include "standardpl/standardpl.h"
#include "eopl/eopl.h"
#include "pairpl/pairpl.h"
#include "arguments/search_args.h"
#include "stickers/stickermap.h"

typedef struct {
    const char * name;
    const char * description;
    int cacheCuboid;
    
    // ** lifecycle **
    
    CLArgumentList * (*default_arguments)();
    // run and resume return 0 on failure
    int (*run)(CLSearchParameters * params, CLArgumentList * args, void ** userData);
    int (*resume)(CLSearchParameters * params, FILE * fp, void ** userData);
    
    void (*save)(void * data, FILE * fp);
    void (*completed)(void * data);
    
    // ** search **
    
    int (*is_goal)(void * data, const Cuboid * cb, Cuboid * cache);
} Solver;

static const Solver SolverTable[] = {
    {
        "standard", "the standard sticker based cuboid solver", 0,
        standardpl_default_arguments,
        standardpl_run,
        standardpl_resume,
        standardpl_save,
        standardpl_completed,
        standardpl_is_goal
     },
     {
         "eo", "solves the edge orientation along one axis", 1,
         eopl_default_arguments,
         eopl_run,
         eopl_resume,
         eopl_save,
         eopl_completed,
         eopl_is_goal
      },
      {
          "pair", "pairs the edges of a cuboid", 0,
          pairpl_default_arguments,
          pairpl_run,
          pairpl_resume,
          pairpl_save,
          pairpl_completed,
          pairpl_is_goal
       }
};

static const int SolverTableCount = 3;

#endif
