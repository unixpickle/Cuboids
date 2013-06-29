/**
 * Center orientations
 */

#include "representation/cuboid_base.h"
#include "algebra/rotation_group.h"
#include "notation/parser.h"
#include "notation/cuboid.h"

typedef struct {
    RotationBasis symmetries;
    uint16_t * x, * y, * z;
} CEOContext;

uint8_t cuboid_center_orientation(int face, uint8_t axis);

CEOContext * ceo_context_create(RotationBasis sym);
int ceo_context_axis_compatibility(CEOContext * context, uint8_t a1, uint8_t a2);
int ceo_context_data_size(CEOContext * context);
void ceo_context_get_data(CEOContext * context, const Cuboid * cb,
                          uint8_t * out, uint8_t axis);
void ceo_context_free(CEOContext * context);
