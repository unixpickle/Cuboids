#include "representation/cuboid_base.h"

/**
 * Calculates whether or not the an edge `edge` at the physical
 * slot `physicalDedge` is correctly oriented if the faces normal
 * to `relativeAxis` are not to be turned.
 * @argument edge The physical edge
 * @argument physicalDedge The physical slot
 * @argument relativeAxis The axis (x = 0, y = 1, z = 2) which
 * is used to define the edge orientation of the piece.
 * @return 0 if the edge is BAD, 1 if GOOD
 */
uint8_t cuboid_edge_orientation(CuboidEdge edge, int physicalDedge,
                                int relativeAxis);

