#include "co.h"

static void _generate_corner_map(const Cuboid * cb, uint8_t * translation);
static uint16_t _corner_orientations(const Cuboid * cb, const uint8_t * map,
                                     uint8_t symmetry, uint8_t axis);

uint8_t cuboid_corner_orientation(uint8_t symmetry, uint8_t axis) {
    uint8_t orientations[3][6] = {
        {0, 1, 0, 2, 1, 2}, // i.e. where is the x sticker pointing
        {1, 0, 2, 1, 2, 0}, // y sticker
        {2, 2, 1, 0, 0, 1} // z sticker
    };
    return orientations[axis][symmetry];
}

COContext * co_context_create(RotationBasis sym) {
    COContext * context = (COContext *)malloc(sizeof(COContext));
    bzero(context, sizeof(COContext));
    context->symmetries = sym;
    CuboidDimensions dims = sym.dims;
    
    Algorithm * alg;
    Cuboid * rot;
    
    if (sym.xPower == 1) {
        alg = algorithm_for_string("x");
        rot = algorithm_to_cuboid(alg, dims);
        algorithm_free(alg);
        context->x = (uint8_t *)malloc(8);
        _generate_corner_map(rot, context->x);
        cuboid_free(rot);
    }
    if (sym.yPower == 1) {
        alg = algorithm_for_string("y");
        rot = algorithm_to_cuboid(alg, dims);
        algorithm_free(alg);
        context->y = (uint8_t *)malloc(8);
        _generate_corner_map(rot, context->y);
        cuboid_free(rot);
    }
    if (sym.zPower == 1) {
        alg = algorithm_for_string("z");
        rot = algorithm_to_cuboid(alg, dims);
        algorithm_free(alg);
        context->z = (uint8_t *)malloc(8);
        _generate_corner_map(rot, context->z);
        cuboid_free(rot);
    }
    
    return context;
}

int co_context_axis_compatibility(COContext * context, uint8_t a1, uint8_t a2) {
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

uint16_t co_context_get_data(COContext * context, const Cuboid * cb, uint8_t axis) {
    uint8_t rotSymmetry = 0;
    const uint8_t * map = NULL;
    if (axis == 1) {
        if (co_context_axis_compatibility(context, 1, 0)) {
            map = context->z;
            rotSymmetry = 1;
            assert(map != NULL);
        }
    } else if (axis == 2) {
        if (co_context_axis_compatibility(context, 2, 0)) {
            map = context->y;
            rotSymmetry = 3;
            assert(map != NULL);
        } else if (co_context_axis_compatibility(context, 2, 1)) {
            map = context->x;
            rotSymmetry = 2;
            assert(map != NULL);
        }
    }
    return _corner_orientations(cb, map, rotSymmetry, axis);
}

void co_context_free(COContext * context) {
    if (context->x) free(context->x);
    if (context->z) free(context->y);
    if (context->y) free(context->z);
    free(context);
}

/***********
 * Private *
 ***********/

static void _generate_corner_map(const Cuboid * cb, uint8_t * translation) {
    int i;
    for (i = 0; i < 8; i++) {
        translation[i] = cb->corners[i].index;
    }
}

static uint16_t _corner_orientations(const Cuboid * cb, const uint8_t * map,
                                     uint8_t symmetry, uint8_t axis) {
    int i;
    uint16_t orientations = 0;
    for (i = 0; i < 8; i++) {
        CuboidCorner c;
        if (!map) {
            c = cb->corners[i];
        } else {
            c = cb->corners[map[i]];
        }
        uint8_t useSym = symmetry3_operation_compose(symmetry, c.symmetry);
        uint8_t orientation = cuboid_corner_orientation(useSym, axis);
        orientations |= (orientation << (i * 2));
    }
    return orientations;
}
