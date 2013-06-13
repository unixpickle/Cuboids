#include "cuboid_moves.h"

FaceMap cuboid_moves_face_map(CuboidMovesAxis axis, int offset) {
    assert(offset == -1 || offset == 1);
    int indexes[3][2] = {{0, 1}, {3, 2}, {5, 4}};
    int index = indexes[axis][(offset + 1) / 2];
    return FacesTable[index];
}

SliceMap cuboid_moves_slice_map(CuboidMovesAxis axis) {
    return SlicesTable[axis];
}

int cuboid_face_for_face_turn(CuboidMovesAxis axis, int offset) {
    assert(offset == -1 || offset == 1);
    int indexes[3][2] = {{6, 5}, {4, 3}, {2, 1}};
    return indexes[axis][(offset + 1) / 2];
}
