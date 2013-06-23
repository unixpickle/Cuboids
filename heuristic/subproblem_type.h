#include "algebra/rotation_group.h"
#include "arguments/arguments.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
    RotationBasis symmetries;
} HSSaveParameters;

typedef struct {
    CuboidDimensions dimensions;
    HSSaveParameters params;
} HSRunParameters;

/***
 * 
 * This structure defines a set of methods which the heuristic indexer
 * will use to generate index files. In addition, this structure will
 * be used during solves to get the heuristic data for each node.
 *
 */
typedef struct {
    const char * name;
    
    CLArgumentList * (*default_arguments)();
    
    /* creates a new indexer with command line arguments */
    int (*initialize)(HSRunParameters params, CLArgumentList * arguments, void ** userData);
    
    /* loads a saved indexer from a file */
    int (*load)(HSSaveParameters params, FILE * fp, void ** userData);
    
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
    
    /* returns the data for a specified angle index; 0 will suffice for this during indexing */
    void (*get_data)(void * userData, const Cuboid * cb, uint8_t * out, int angle);
    
    /* called to indicate that the heuristic is no longer needed */
    void (*completed)(void * userData);
} HSubproblem;
