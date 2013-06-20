#include "basis.h"

void _cuboid_generate_turns(AlgList * list, CuboidDimensions dims,
                            char face, int layers, int quarters);

AlgList * cuboid_standard_axis_basis(CuboidDimensions dims, CuboidMovesAxis axis) {
    char faceNames[] = {'F', 'B', 'U', 'D', 'R', 'L'};
    
    int sideLength = 0;
    if (axis == CuboidMovesAxisX) sideLength = dims.x;
    if (axis == CuboidMovesAxisY) sideLength = dims.y;
    if (axis == CuboidMovesAxisZ) sideLength = dims.z;
    
    AlgList * list = alg_list_create();
    
    int quarter = cuboid_validate_quarter_turn(dims, axis);
    int i;
    
    for (i = 1; i <= sideLength / 2; i++) {
        int negFace = cuboid_face_for_face_turn(axis, -1);
        int posFace = cuboid_face_for_face_turn(axis, 1);
        _cuboid_generate_turns(list, dims, faceNames[negFace - 1], i, quarter);
        _cuboid_generate_turns(list, dims, faceNames[posFace - 1], i, quarter);
    }
    
    return list;
}

AlgList * cuboid_standard_basis(CuboidDimensions dims) {
    AlgList * collective = cuboid_standard_axis_basis(dims, CuboidMovesAxisX);
    int axis, i;
    for (axis = 1; axis < 3; axis++) {
        // somewhat hacky, but this is a decent & efficient way to do it
        AlgList * addition = cuboid_standard_axis_basis(dims, axis);
        for (i = 0; i < addition->entryCount; i++) {
            alg_list_add(collective, addition->entries[i]);
        }
        free(addition->entries);
        free(addition);
    }
    return collective;
}

/***********
 * Private *
 ***********/

void _cuboid_generate_turns(AlgList * list, CuboidDimensions dims,
                            char face, int layers, int quarters) {
    AlgListEntry entry;
                                
    Algorithm * halfTurn = algorithm_new_wide_turn(face, layers);
    halfTurn->power = 2;
    Cuboid * halfCuboid = algorithm_to_cuboid(halfTurn, dims);
    entry.algorithm = halfTurn;
    entry.cuboid = halfCuboid;
    alg_list_add(list, entry);
    
    if (!quarters) return;
    
    Algorithm * quarterTurn = algorithm_new_wide_turn(face, layers);
    Cuboid * quarterCuboid = algorithm_to_cuboid(quarterTurn, dims);
    entry.algorithm = quarterTurn;
    entry.cuboid = quarterCuboid;
    alg_list_add(list, entry);
    
    Algorithm * quarterInv = algorithm_new_wide_turn(face, layers);
    quarterInv->inverseFlag = 1;
    Cuboid * quarterInvCuboid = algorithm_to_cuboid(quarterInv, dims);
    entry.algorithm = quarterInv;
    entry.cuboid = quarterInvCuboid;
    alg_list_add(list, entry);    
}
