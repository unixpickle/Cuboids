#include "representation/cuboid_base.h"
#include "eo_table.h"
#include "algebra/rotation_group.h"
#include "notation/parser.h"

typedef struct {
    RotationBasis symmetries;
    // edge coordinate translations
    uint16_t * x, * y, * z;
} EOContext;

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

EOContext * eo_context_create(RotationBasis symmetries);
int eo_context_axis_compatibility(EOContext * context, int a1, int a2);
int eo_context_compact_data_length(EOContext * context);
void eo_context_get_compact_data(EOContext * context, const Cuboid * cb,
                                 uint8_t * out, int axis);
void eo_context_free(EOContext * context);
