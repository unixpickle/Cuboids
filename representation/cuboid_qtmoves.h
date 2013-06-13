#include "cuboid_moves.h"
#include "cuboid_base.h"

/*

STANDARDS TO NOTE:

- Turns occur around three different axes:
    R and L turns revolve around X
    U and D turns revolve around Y
    F and B turns revolve around Z
- A slice turn has slice index, aka the slice layer
    On a 3x3x3, all slices are of index 0
    Slice offsets are indexed the same way edges are; the
    slice offset corresponds to the index of the edges
    which will be affected by the slice.
- A quarter turn has a direction
    All basic quarter turns returned by this function obey the following rules:
        * X axis - turns follow L on a 3x3x3
        * Y axis - turns follow U on a 3x3x3
        * Z axis - turns follow F on a 3x3x3
- A quarter turn is either positive or negative:
    A positive quarter turn affects the F, R, or U face
    A negative quarter turn affects the B, L, or D face
- Examples of turns:
    On a 4x4, slicing the right inner slice is doing an X turn with an offset of 1
    On an 8x8, doing B' is doing a negative Z quarter turn

*/

int cuboid_validate_quarter_turn(CuboidDimensions dimensions,
                                 CuboidMovesAxis axis);

Cuboid * cuboid_quarter_face_turn(CuboidDimensions dimensions,
                                           CuboidMovesAxis axis,
                                           int offset);

Cuboid * cuboid_quarter_slice(CuboidDimensions dimensions,
                                       CuboidMovesAxis axis,
                                       int layer);
