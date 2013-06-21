#ifndef __EO_TABLE_H__
#define __EO_TABLE_H__

#include <stdint.h>

// symmetries taken from representation/symmetry3.h

typedef struct {
    // 1 if the dedge at the index is on the defined slice
    // for the axis, 0 if in the sandwich layers
    uint8_t dedgesAreSlice[12];
    
    // NOTE: all symmetry lists exclude the obvious identity symmetry
    
    // the symmetries which a slice edge will have it it is
    // GOOD and located on a sandwich layer
    uint8_t sliceOnSandwich[2];
    
    // the symmetry which a sandwich edge will have if it
    // is is GOOD and located on a sandwich layer
    uint8_t sandwichOnSandwich;
    
    // the symmetries which a sandwich edge will have if it
    // is GOOD and located on a slice layer
    uint8_t sandwichOnSlice[2];
} EOAxisInfo;

static const EOAxisInfo EOAxisTable[3] = {
    { // R and L turns
        {0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0}, // same as F and B
        {1, 4}, // (2, 1, 3), (3, 1, 2)
        3, // (3, 2, 1)
        {1, 5}, // (2, 1, 3), (2, 3, 1)
    },
    { // U and D turns
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1}, // S slice, typed super quick
        {2, 4}, // (1, 3, 2), (3, 1, 2)
        1, // (2, 1, 3)
        {2, 5} // (1, 3, 2), (2, 3, 1)
    },
    { // F and B turns
        {0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0}, // I typed this real fast...
        {2, 5}, // (1, 3, 2), (2, 3, 1)
        3, // (3, 2, 1)
        {2, 4} // (1, 3, 2), (3, 1, 2)
    },
};

#endif