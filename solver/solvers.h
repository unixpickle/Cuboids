/**
 * This is essentially an interface which all solvers must
 * implement in order to be registered with the main program.
 */

#include <stdio.h>

#include "standardpl/standardpl.h"
#include "arguments/search_args.h"
#include "stickers/stickermap.h"

typedef struct {
    const char * name;
    
    // ** lifecycle **
    
    CLArgumentList * (*default_arguments)();
    // run and resume return 0 on failure
    int (*run)(CLSearchParameters * params, CLArgumentList * args, void ** userData);
    int (*resume)(CLSearchParameters * params, FILE * fp, void ** userData);
    
    void (*save)(void * data, FILE * fp);
    void (*completed)(void * data);
    
    // ** search **
    
    int (*is_goal)(void * data, Cuboid * cb, StickerMap * cache);
} Solver;

static const Solver SolverTable[] = {
    {
        "standard",
        standardpl_default_arguments,
        standardpl_run,
        standardpl_resume,
        standardpl_save,
        standardpl_completed,
        standardpl_is_goal
     }
};

static const int SolverTableCount = 1;
