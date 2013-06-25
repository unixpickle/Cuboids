#include "eo_index.h"

typedef struct {
    RotationBasis symmetries;
    uint16_t * xRotation;
    uint16_t * yRotation;
    uint16_t * zRotation;
} EOIndexData;

static void _generate_rotations(EOIndexData * data);
static void _rotation_to_map(const Cuboid * rotation, uint16_t * edgeSlots);
static void _generate_edge_orientation_data(uint8_t * data, const uint16_t * rotation,
                                            const Cuboid * cuboid, int axis);

CLArgumentList * eo_index_default_arguments() {
    return cl_argument_list_new();
}

int eo_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData) {
    EOIndexData * data = (EOIndexData *)malloc(sizeof(EOIndexData));
    data->symmetries = params.symmetries;
    *userData = data;
    _generate_rotations(data);
    return 1;
}

int eo_index_load(HSParameters params, FILE * fp, void ** userData) {
    RotationBasis symmetries;
    uint8_t x, y, z;
    if (!load_uint8(&x, fp)) return 0;
    if (!load_uint8(&y, fp)) return 0;
    if (!load_uint8(&z, fp)) return 0;
    if (!load_cuboid_dimensions(&symmetries.dims, fp)) return 0;
    if (x != params.symmetries.xPower) return 0;
    if (y != params.symmetries.yPower) return 0;
    if (z != params.symmetries.zPower) return 0;
    if (!cuboid_dimensions_equal(symmetries.dims, params.symmetries.dims)) {
        return 0;
    }
    
    symmetries.xPower = x;
    symmetries.yPower = y;
    symmetries.zPower = z;
    EOIndexData * data = (EOIndexData *)malloc(sizeof(EOIndexData));
    data->symmetries = symmetries;
    *userData = data;
    
    _generate_rotations(data);
    return 1;
}

void eo_index_save(void * userData, FILE * fp) {
    EOIndexData * data = (EOIndexData *)userData;
    save_uint8(data->symmetries.xPower, fp);
    save_uint8(data->symmetries.yPower, fp);
    save_uint8(data->symmetries.zPower, fp);
    save_cuboid_dimensions(data->symmetries.dims, fp);
}

int eo_index_supports_dimensions(void * userData, CuboidDimensions dims) {
    EOIndexData * data = (EOIndexData *)userData;
    return cuboid_dimensions_equal(data->symmetries.dims, dims);
}

int eo_index_data_size(void * userData) {
    EOIndexData * data = (EOIndexData *)userData;
    CuboidDimensions dims = data->symmetries.dims;
    int edgeCount = 0;
    edgeCount += (dims.x - 2) * 4;
    edgeCount += (dims.y - 2) * 4;
    edgeCount += (dims.z - 2) * 4;
    
    if (edgeCount % 8 == 0) return edgeCount / 8;
    else return edgeCount / 8 + 1;
}

int eo_index_angle_count(void * userData) {
    return 3;
}

int eo_index_angles_are_equivalent(void * userData, int a1, int a2) {
    assert(a1 != a2);
    assert(a1 < 3);
    assert(a2 < 3);
    
    EOIndexData * data = (EOIndexData *)userData;
    
    int lower = a1 < a2 ? a1 : a2;
    int higher = a1 < a2 ? a2 : a1;
    if (lower == 0) {
        if (higher == 1) {
            // going from x to y
            return data->symmetries.zPower == 1;
        } else {
            // going from x to z
            return data->symmetries.yPower == 1;
        }
    } else {
        // going from y to z
        return data->symmetries.xPower == 1;
    }
}

void eo_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle) {
    EOIndexData * data = (EOIndexData *)userData;
    uint16_t * dedgeRotation = NULL;
    // use the angle as our way of determining the rotation to use
    if (angle == 1) {
        if (eo_index_angles_are_equivalent(userData, 1, 0)) {
            dedgeRotation = data->zRotation;
        }
    } else if (angle == 2) {
        if (eo_index_angles_are_equivalent(userData, 2, 0)) {
            dedgeRotation = data->yRotation;
        } else if (eo_index_angles_are_equivalent(userData, 2, 1)) {
            dedgeRotation = data->xRotation;
        }
    }
    bzero(out, eo_index_data_size(userData));
    _generate_edge_orientation_data(out, dedgeRotation, cb, angle);
}

void eo_index_completed(void * userData) {
    EOIndexData * data = (EOIndexData *)userData;
    
    free(data->xRotation);
    free(data->yRotation);
    free(data->zRotation);
    free(data);
}

/***********
 * Private *
 ***********/

static void _generate_rotations(EOIndexData * data) {
    CuboidDimensions dims = data->symmetries.dims;
    
    Algorithm * alg = algorithm_for_string("x");
    Cuboid * rot = algorithm_to_cuboid(alg, dims);
    algorithm_free(alg);
    
    int edgeCount = cuboid_count_edges(rot);
    data->xRotation = (uint16_t *)malloc(sizeof(uint16_t) * edgeCount);
    data->yRotation = (uint16_t *)malloc(sizeof(uint16_t) * edgeCount);
    data->zRotation = (uint16_t *)malloc(sizeof(uint16_t) * edgeCount);
    
    _rotation_to_map(rot, data->xRotation);
    cuboid_free(rot);
    
    alg = algorithm_for_string("y");
    rot = algorithm_to_cuboid(alg, dims);
    algorithm_free(alg);
    _rotation_to_map(rot, data->yRotation);
    cuboid_free(rot);
    
    alg = algorithm_for_string("z");
    rot = algorithm_to_cuboid(alg, dims);
    algorithm_free(alg);
    _rotation_to_map(rot, data->zRotation);
    cuboid_free(rot);
}

static void _rotation_to_map(const Cuboid * rotation, uint16_t * edgeSlots) {
    int i;
    for (i = 0; i < cuboid_count_edges(rotation); i++) {
        CuboidEdge edge = rotation->edges[i];
        int piece = cuboid_edge_index(rotation, edge.dedgeIndex, edge.edgeIndex);
        edgeSlots[i] = (uint16_t)piece;
    }
}

static void _generate_edge_orientation_data(uint8_t * data, const uint16_t * rotation,
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
                data[byteIndex] |= (1 << bitIndex);
            }
        }
    }
}
