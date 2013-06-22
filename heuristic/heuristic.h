#include "arguments/search_args.h"
#include <stdint.h>

/***
 * 
 * This structure defines a set of methods which the heuristic indexer
 * will use to generate index files. In addition, this structure will
 * be used during solves to get the heuristic data for each node.
 *
 */
typedef struct {
    CLArgumentList * (*default_arguments)();
    
    /* creates a new indexer with command line arguments */
    int (*run)(CLSearchParameters * params, CLArgumentList * arguments, void ** userData);
    
    /* loads a saved indexer from a file */
    int (*load)(CLSearchParameters * params, FILE * fp, void ** userData);
    
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
} Heuristic;
