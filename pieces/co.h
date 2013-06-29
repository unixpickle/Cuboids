/**
 * Corner orientations
 */

#include "representation/cuboid_base.h"
#include "algebra/rotation_group.h"
#include "notation/parser.h"
#include "notation/cuboid.h"

typedef struct {
    RotationBasis symmetries;
    uint8_t * x, * y, * z;
} COContext;

uint8_t cuboid_corner_orientation(uint8_t symmetry, uint8_t axis);

COContext * co_context_create(RotationBasis sym);
int co_context_axis_compatibility(COContext * context, uint8_t a1, uint8_t a2);
uint16_t co_context_get_data(COContext * context, const Cuboid * cb, uint8_t axis);
void co_context_free(COContext * context);
