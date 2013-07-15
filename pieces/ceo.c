#include "ceo.h"

static void _generate_center_map(const Cuboid * cuboid, uint16_t * centers);
static void _generate_ceo_data(const Cuboid * cuboid, uint8_t * out,
                               const uint16_t * map, uint8_t axis);

uint8_t cuboid_center_orientation(int face, uint8_t axis) {
    const static uint8_t axisFaces[3][2] = {{5, 6}, {3, 4}, {1, 2}};
    return (face == axisFaces[axis][0] || face == axisFaces[axis][1]);
}

CEOContext * ceo_context_create(RotationBasis sym) {
    CEOContext * context = (CEOContext *)malloc(sizeof(CEOContext));
    bzero(context, sizeof(CEOContext));
    context->symmetries = sym;
    CuboidDimensions dims = sym.dims;
    
    Algorithm * alg;
    Cuboid * rot;
    
    if (sym.xPower == 1) {
        alg = algorithm_for_string("x");
        rot = algorithm_to_cuboid(alg, dims);
        algorithm_free(alg);
        int centerCount = cuboid_count_centers(rot);
        context->x = (uint16_t *)malloc(centerCount * sizeof(uint16_t));
        _generate_center_map(rot, context->x);
        cuboid_free(rot);
    }
    if (sym.yPower == 1) {
        alg = algorithm_for_string("y");
        rot = algorithm_to_cuboid(alg, dims);
        algorithm_free(alg);
        int centerCount = cuboid_count_centers(rot);
        context->y = (uint16_t *)malloc(centerCount * sizeof(uint16_t));
        _generate_center_map(rot, context->y);
        cuboid_free(rot);
    }
    if (sym.zPower == 1) {
        alg = algorithm_for_string("z");
        rot = algorithm_to_cuboid(alg, dims);
        algorithm_free(alg);
        int centerCount = cuboid_count_centers(rot);
        context->z = (uint16_t *)malloc(centerCount * sizeof(uint16_t));
        _generate_center_map(rot, context->z);
        cuboid_free(rot);
    }
    
    return context;
}

int ceo_context_axis_compatibility(CEOContext * context, uint8_t a1, uint8_t a2) {
    assert(a1 != a2);
    assert(a1 < 3);
    assert(a2 < 3);
        
    int lower = a1 < a2 ? a1 : a2;
    int higher = a1 < a2 ? a2 : a1;
    if (lower == 0) {
        if (higher == 1) {
            // going from x to y
            return (context->z != NULL);
        } else {
            // going from x to z
            return (context->y != NULL);
        }
    } else {
        // going from y to z
        return (context->x != NULL);
    }
}

int ceo_context_data_size(CEOContext * context) {
    CuboidDimensions dims = context->symmetries.dims;
    int centerCount = 2 * (dims.x - 2) * (dims.y - 2)
                      + 2 * (dims.z - 2) * (dims.y - 2)
                      + 2 * (dims.x - 2) * (dims.z - 2);
    
    // return 2 + ceil(centerCount / 8)
    if (centerCount % 8 == 0) return centerCount / 8;
    else return 1 + centerCount / 8;
}

void ceo_context_get_data(CEOContext * context, const Cuboid * cb,
                          uint8_t * out, uint8_t axis) {
    const uint16_t * map = NULL;
    if (axis == 1) {
      if (ceo_context_axis_compatibility(context, 1, 0)) {
          map = context->z;
          assert(map != NULL);
      }
    } else if (axis == 2) {
      if (ceo_context_axis_compatibility(context, 2, 0)) {
          map = context->y;
          assert(map != NULL);
      } else if (ceo_context_axis_compatibility(context, 2, 1)) {
          map = context->x;
          assert(map != NULL);
      }
    }
    _generate_ceo_data(cb, out, map, axis);
}

void ceo_context_free(CEOContext * context) {
    if (context->x) free(context->x);
    if (context->z) free(context->y);
    if (context->y) free(context->z);
    free(context);
}

/***********
 * Private *
 ***********/

static void _generate_center_map(const Cuboid * cuboid, uint16_t * centers) {
    int i, centerCount = cuboid_count_centers(cuboid);
    for (i = 0; i < centerCount; i++) {
        CuboidCenter c = cuboid->centers[i];
        centers[i] = cuboid_center_index(cuboid, c.side, c.index);
    }
}

static void _generate_ceo_data(const Cuboid * cuboid, uint8_t * out,
                               const uint16_t * map, uint8_t axis) {
    int i, count = cuboid_count_centers(cuboid);
    for (i = 0; i < count; i++) {
        int byteIndex = i / 8;
        int bitIndex = i % 8;
        CuboidCenter c;
        if (map) {
            c = cuboid->centers[map[i]];
        } else {
            c = cuboid->centers[i];
        }
        uint8_t orientation = cuboid_center_orientation(c.side, axis);
        if (orientation) {
            out[byteIndex] |= (1 << bitIndex);
        }
    }
}
