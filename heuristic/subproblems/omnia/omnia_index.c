#include "omnia_index.h"

static void _omnia_copy_corners(uint8_t * data, const Cuboid * cb);
static void _omnia_copy_centers(uint8_t * data, const Cuboid * cb);
static void _omnia_copy_edges(uint8_t * data, const Cuboid * cb);

CLArgumentList * omnia_index_default_arguments() {
    return cl_argument_list_new();
}

int omnia_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData) {
    CuboidDimensions * dims = (CuboidDimensions *)malloc(sizeof(CuboidDimensions));
    *dims = params.symmetries.dims;
    *userData = dims;
    return 1;
}

int omnia_index_load(HSParameters params, FILE * fp, void ** userData) {
    CuboidDimensions * dims = (CuboidDimensions *)malloc(sizeof(CuboidDimensions));
    if (!load_cuboid_dimensions(dims, fp)) { 
        free(dims);
        return 0;
    }
    if (!omnia_index_supports_dimensions(dims, params.symmetries.dims)) {
        free(dims);
        return 0;
    }
    *userData = dims;
    return 1;
}

void omnia_index_save(void * userData, FILE * fp) {
    CuboidDimensions dims = *((CuboidDimensions *)userData);
    save_cuboid_dimensions(dims, fp);
}

int omnia_index_supports_dimensions(void * userData, CuboidDimensions theDims) {
    CuboidDimensions * dims = (CuboidDimensions *)userData;
    return cuboid_dimensions_equal(*dims, theDims);
}

int omnia_index_data_size(void * userData) {
    CuboidDimensions dims = *((CuboidDimensions *)userData);
    int edgeCount = 4 * (dims.x - 2) + 4 * (dims.y - 2) + 4 * (dims.z - 2);
    int centerCount = 2 * (dims.x - 2) * (dims.y - 2)
                      + 2 * (dims.z - 2) * (dims.y - 2)
                      + 2 * (dims.x - 2) * (dims.z - 2);
    return 8 + edgeCount + centerCount;
}

int omnia_index_angle_count(void * userData) {
    return 1;
}

int omnia_index_angles_are_equivalent(void * userData, int a1, int a2) {
    puts("omnia_index_angles_are_equivalent() should never be called.");
    abort();
}

void omnia_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle) {
    int centerCount = cuboid_count_centers(cb);
    bzero(out, omnia_index_data_size(userData));
    _omnia_copy_corners(out, cb);
    _omnia_copy_centers(&out[8], cb);
    _omnia_copy_edges(&out[8 + centerCount], cb);
}

void omnia_index_completed(void * userData) {
    free(userData);
}

static void _omnia_copy_corners(uint8_t * data, const Cuboid * cb) {
    int i;
    for (i = 0; i < 8; i++) {
        CuboidCorner c = cb->corners[i];
        data[i] = c.index | (c.symmetry << 4);
    }
}

static void _omnia_copy_centers(uint8_t * data, const Cuboid * cb) {
    int i;
    int centerCount = cuboid_count_centers(cb);
    for (i = 0; i < centerCount; i++) {
        CuboidCenter c = cb->centers[i];
        data[i] = c.side;
    }
}

static void _omnia_copy_edges(uint8_t * data, const Cuboid * cb) {
    int i;
    int edgeCount = cuboid_count_edges(cb);
    for (i = 0; i < edgeCount; i++) {
        CuboidEdge e = cb->edges[i];
        data[i] = e.dedgeIndex | (e.symmetry << 4);
    }
}
