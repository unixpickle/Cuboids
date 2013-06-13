#include "cuboid_base.h"
#include "cuboid_moves.h"

/**
 * Double moves are possible on all faces of a cuboid.
 *
 * See cuboid_qtmoves.h for standards.
 */

Cuboid * cuboid_half_face_turn(CuboidDimensions dims, CuboidMovesAxis axis, int offset);
Cuboid * cuboid_half_slice(CuboidDimensions dims, CuboidMovesAxis axis, int layer);
