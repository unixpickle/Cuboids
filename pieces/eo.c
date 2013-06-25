#include "eo.h"

static void _rotation_to_map(const Cuboid * rotation, uint16_t * edgeSlots);
static void _generate_edge_data(uint8_t * out, uint16_t * rotation,
                                const Cuboid * cb, int axis);

uint8_t cuboid_edge_orientation(CuboidEdge edge, int physicalDedge,
                                int relativeAxis) {
    if (edge.symmetry == 0) return 1;
    EOAxisInfo axis = EOAxisTable[relativeAxis];
    uint8_t slotSlice = axis.dedgesAreSlice[physicalDedge];
    uint8_t pieceSlice = axis.dedgesAreSlice[edge.dedgeIndex];
    
    // super DUPER confusing nested if stuff
    if (slotSlice) {
        if (pieceSlice) return 0;
        else {
            // sandwich on slice
            if (axis.sandwichOnSlice[0] == edge.symmetry ||
                axis.sandwichOnSlice[1] == edge.symmetry) {
                return 1;
            } else return 0;
        }
    } else {
        if (pieceSlice) {
            // slice on sandwich
            if (axis.sliceOnSandwich[0] == edge.symmetry ||
                axis.sliceOnSandwich[1] == edge.symmetry) {
                return 1;
            } else return 0;
        } else {
            // sandwich on sandwich
            return (edge.symmetry == axis.sandwichOnSandwich);
        }
    }
}

EOContext * eo_context_create(RotationBasis symmetries) {
    CuboidDimensions dims = symmetries.dims;
    EOContext * context = (EOContext *)malloc(sizeof(EOContext));
    bzero(context, sizeof(EOContext));
    
    Algorithm * alg;
    Cuboid * rot;
    
    context->symmetries = symmetries;
    
    if (symmetries.xPower == 1) {
        alg = algorithm_for_string("x");
        rot = algorithm_to_cuboid(alg, dims);
        algorithm_free(alg);
        int edgeCount = cuboid_count_edges(rot);
        context->x = (uint16_t *)malloc(sizeof(uint16_t) * edgeCount);
        _rotation_to_map(rot, context->x);
        cuboid_free(rot);
    }
    if (symmetries.yPower == 1) {
        alg = algorithm_for_string("y");
        rot = algorithm_to_cuboid(alg, dims);
        algorithm_free(alg);
        int edgeCount = cuboid_count_edges(rot);
        context->y = (uint16_t *)malloc(sizeof(uint16_t) * edgeCount);
        _rotation_to_map(rot, context->y);
        cuboid_free(rot);
    }
    if (symmetries.zPower == 1) {
        alg = algorithm_for_string("z");
        rot = algorithm_to_cuboid(alg, dims);
        algorithm_free(alg);
        int edgeCount = cuboid_count_edges(rot);
        context->z = (uint16_t *)malloc(sizeof(uint16_t) * edgeCount);
        _rotation_to_map(rot, context->z);
        cuboid_free(rot);
    }
        
    return context;
}

int eo_context_axis_compatibility(EOContext * context, int a1, int a2) {
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

int eo_context_compact_data_length(EOContext * context) {
    CuboidDimensions dims = context->symmetries.dims;
    int edgeCount = 0;
    edgeCount += (dims.x - 2) * 4;
    edgeCount += (dims.y - 2) * 4;
    edgeCount += (dims.z - 2) * 4;
    
    if (edgeCount % 8 == 0) return edgeCount / 8;
    else return edgeCount / 8 + 1;
}

void eo_context_get_compact_data(EOContext * context, const Cuboid * cb,
                                 uint8_t * out, int axis) {
    uint16_t * dedgeRotation = NULL;
    // use the axis as our way of determining the rotation to use
    if (axis == 1) {
        if (eo_context_axis_compatibility(context, 1, 0)) {
            dedgeRotation = context->z;
            assert(dedgeRotation != NULL);
        }
    } else if (axis == 2) {
        if (eo_context_axis_compatibility(context, 2, 0)) {
            dedgeRotation = context->y;
            assert(dedgeRotation != NULL);
        } else if (eo_context_axis_compatibility(context, 2, 1)) {
            dedgeRotation = context->x;
            assert(dedgeRotation != NULL);
        }
    }
    bzero(out, eo_context_compact_data_length(context));
    _generate_edge_data(out, dedgeRotation, cb, axis);
}

void eo_context_free(EOContext * context) {
    if (context->x) free(context->x);
    if (context->y) free(context->y);
    if (context->z) free(context->z);
    free(context);
}

/***********
 * Private *
 ***********/

static void _rotation_to_map(const Cuboid * rotation, uint16_t * edgeSlots) {
    int i;
    for (i = 0; i < cuboid_count_edges(rotation); i++) {
        CuboidEdge edge = rotation->edges[i];
        int piece = cuboid_edge_index(rotation, edge.dedgeIndex, edge.edgeIndex);
        edgeSlots[i] = (uint16_t)piece;
    }
}

static void _generate_edge_data(uint8_t * out, uint16_t * rotation,
                                const Cuboid * cuboid, int axis) {
    int dedgeIndex, edgeIndex;
    for (dedgeIndex = 0; dedgeIndex < 12; dedgeIndex++) {
        int edgeCount = cuboid_count_edges_for_dedge(cuboid, dedgeIndex);
        for (edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++) {
            int i = cuboid_edge_index(cuboid, dedgeIndex, edgeIndex);
            int byteIndex = i / 8;
            int bitIndex = i % 8;
            
            // index is the physical slot; might be translated
            int index = i;
            if (rotation) {
                index = rotation[i];
            }
        
            // get the edge orientation
            CuboidEdge edge = cuboid->edges[index];
            
            // although we made the translation, we will still get the orientation
            // as if the edge were in the source dedge
            int orientation = cuboid_edge_orientation(edge, dedgeIndex, axis);
            if (orientation) {
                out[byteIndex] |= (1 << bitIndex);
            }
        }
    }
}
