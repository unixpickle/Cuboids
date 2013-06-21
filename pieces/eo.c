#include "eo.h"

uint8_t cuboid_edge_orientation(CuboidEdge edge, int physicalDedge,
                                int relativeAxis) {
    if (edge.symmetry == 0) return 1;
    EOAxisInfo axis = EOAxisTable[relativeAxis];
    uint8_t slotSlice = axis.dedgesAreSlice[physicalDedge];
    uint8_t pieceSlice = axis.dedgesAreSlice[edge.dedgeIndex];
    
    // super DUPER confusing nested if stuff
    if (slotSlice) {
        if (pieceSlice) return 0;
        else {
            // sandwich on slice
            if (axis.sandwichOnSlice[0] == edge.symmetry ||
                axis.sandwichOnSlice[1] == edge.symmetry) {
                return 1;
            } else return 0;
        }
    } else {
        if (pieceSlice) {
            // slice on sandwich
            if (axis.sliceOnSandwich[0] == edge.symmetry ||
                axis.sliceOnSandwich[1] == edge.symmetry) {
                return 1;
            } else return 0;
        } else {
            // sandwich on sandwich
            return (edge.symmetry == axis.sandwichOnSandwich);
        }
    }
}
