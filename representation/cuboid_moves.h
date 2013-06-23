#ifndef __CUBOID_MOVES_H__
#define __CUBOID_MOVES_H__

#include "cuboid_moves_table.h"
#include "cuboid_center_turns.h"
#include <assert.h>

typedef enum {
    CuboidMovesAxisX = 0,
    CuboidMovesAxisY = 1,
    CuboidMovesAxisZ = 2
} CuboidMovesAxis;

FaceMap cuboid_moves_face_map(CuboidMovesAxis axis, int offset);
SliceMap cuboid_moves_slice_map(CuboidMovesAxis axis);
int cuboid_face_for_face_turn(CuboidMovesAxis axis, int offset);

int cuboid_slice_center_line_length(CuboidDimensions dim,
                                    SliceMap sliceMap,
                                    int centerIndex);
int cuboid_slice_center_line_index(CuboidDimensions dim,
                                   SliceMap sliceMap,
                                   int centerIndex, // 0-3
                                   int lineIndex,
                                   int indexInLine);


#endif