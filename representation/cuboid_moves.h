#include "cuboid_base.h"

/*

STANDARDS TO NOTE:

- Turns occur around three different axes:
    R and L turns revolve around X
    U and D turns revolve around Y
    F and B turns revolve around Z
- A turn has slice offset
    On a 3x3x3, face turns are offset 1 || -1 and slices have an offset of 0.
    On an even cube, there are no faces with 0 offset
    Positive offset indicates the following:
        * X axis - positive offset indicates a turn on the right side
        * Y axis - positive offset indicates a turn on the upper side
        * Z axis - positive offset indicates a turn on the front side
- A quarter turn has a direction
    All basic quarter turns returned by this function obey the following rules:
        * X axis - turns follow L on a 3x3x3
        * Y axis - turns follow U on a 3x3x3
        * Z axis - turns follow F on a 3x3x3
- Examples of turns:
    On a 4x4, slicing the right inner slice is doing an X turn with an offset of 1
    On an 8x8, doing B' is doing a Z turn with offset -4 with the standard quarter turn direction.

*/

typedef enum {
    CuboidMovesAxisX,
    CuboidMovesAxisY,
    CuboidMovesAxisZ
} CuboidMovesAxis;

int cuboid_validate_quarter_turn(CuboidDimensions dimensions,
                                 CuboidMovesAxis axis);

Cuboid * cuboid_generate_quarter_face_turn(CuboidDimensions dimensions,
                                           CuboidMovesAxis axis,
                                           int offset);

Cuboid * cuboid_generate_quarter_turn(CuboidDimensions dimensions
                                      CuboidMovesAxis axis,
                                      int sliceOffset);
