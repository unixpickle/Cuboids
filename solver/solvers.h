/**
 * This is essentially an interface which all solvers must
 * implement in order to be registered with the main program.
 */

#include <stdio.h>

#include "standardpl.h"

typedef struct {
    const char * name;
    
    // lifecycle
    CLArgumentList * (*default_arguments)();
    int (*run)(CLArgumentList * args, void ** userData); // return 0 on failure
    int (*resume)(void * data, FILE * fp, void ** userData); // ^^^
    void (*save)(void * data, FILE * fp);
    void (*completed)(void * data);
    
    // search
    int (*is_goal)(void * data, Cuboid * cb, StickerMap * cache);
} Solver;

static const Solver[] SolverTable = {
    {"standard",
     
     }
}
