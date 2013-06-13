#ifndef __CUBOID_MOVES_H__
#define __CUBOID_MOVES_H__

#include "cuboid_moves_table.h"
#include <assert.h>

typedef enum {
    CuboidMovesAxisX,
    CuboidMovesAxisY,
    CuboidMovesAxisZ
} CuboidMovesAxis;

FaceMap cuboid_moves_face_map(CuboidMovesAxis axis, int offset);
SliceMap cuboid_moves_slice_map(CuboidMovesAxis axis);
int cuboid_face_for_face_turn(CuboidMovesAxis axis, int offset);

#endif