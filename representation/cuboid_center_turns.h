#include "cuboid_base.h"

/**
 * @argument size The side length of the center
 */

void cuboid_center_dimensions(CuboidDimensions dim, int face, int * w, int * h);

int qt_center_point_to_index(int size, int x, int y);
void qt_center_point_from_index(int size, int index, int * x, int * y);
void qt_rotate_center_point(int face, int * x, int * y);
