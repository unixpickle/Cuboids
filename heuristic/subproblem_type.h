#ifndef __SUBPROBLEM_TYPE_H__
#define __SUBPROBLEM_TYPE_H__

#include "algebra/rotation_group.h"
#include "arguments/arguments.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
    RotationBasis symmetries;
    int maxDepth;
} HSParameters;

/***
 * 
 * This structure defines a set of methods which the heuristic indexer
 * will use to generate index files. In addition, this structure will
 * be used during solves to get the heuristic data for each node.
 *
 */
typedef struct {
    const char * name;
    const char * description;
    
    CLArgumentList * (*default_arguments)();
    
    /* creates a new indexer with command line arguments */
    int (*initialize)(HSParameters params, CLArgumentList * arguments, void ** userData);
    
    /* loads a saved indexer from a file */
    int (*load)(HSParameters params, FILE * fp, void ** userData);
    
    /* saves an indexer to a file */
    void (*save)(void * userData, FILE * fp);
    
    /* checks if an initialized or loaded heuristic will be compatible with a cuboid
     * of a different size. */
    int (*supports_dimensions)(void * userData, CuboidDimensions dims);
    
    /* returns the size of the index data for a cube */
    int (*data_size)(void * userData);
    
    /*
     * returns the number of ways the index data could be generated.
     * for corners, this would be 1. 
     * for something like EO or R2F2U, this would be 3.
     */
    int (*angle_count)(void * userData);
    
    /*
     * returns whether an angle can be saved poperly indexed by a different
     * angle given the parameters which the subproblem was initialized with.
     */
    int (*angles_are_equivalent)(void * userData, int angle1, int angle2);
    
    /* returns the data for a specified angle index; 0 will suffice for this during indexing */
    void (*get_data)(void * userData, const Cuboid * cb, uint8_t * out, int angle);
    
    /* called to indicate that the heuristic is no longer needed */
    void (*completed)(void * userData);
    
    RotationBasis (*data_symmetries)(void * userData);
} HSubproblem;

#endif
