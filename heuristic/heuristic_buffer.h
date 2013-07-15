/***
 * A heuristic buffer consumes a set of rotated cuboids
 * and ultimately sifts out the heuristic value to use.
 *
 * For each cuboid passed in, the caller must also be aware
 * of the coset index to which the passed cuboid belongs.
 * This is because cuboids are sorted in such a way that the
 * "greatest" index data is used for the heuristic.
 */

#include "heuristic.h"

typedef struct {
    // data for each situation
    // the array is dimension'd as follows:
    // data[angles][dataCosets][dataLength]
    uint8_t *** data;
    
    int angleCount;
    int cosetCount;
    int dataSize;
    Heuristic * heuristic;
} HeuristicBuffer;

// Each time a heuristic lookup occurs, the HeuristicBuffer is populated
// with tons of heuristic data for each angle and dataCoset. This data
// is then used to determine the highest possible heuristic value to return.
HeuristicBuffer * heuristic_buffer_create(Heuristic * heuristic);
void heuristic_buffer_free(HeuristicBuffer * buffer);
void heuristic_buffer_add(HeuristicBuffer * buffer, const Cuboid * cb, int coset);
int heuristic_buffer_pruning_value(HeuristicBuffer * buffer);
