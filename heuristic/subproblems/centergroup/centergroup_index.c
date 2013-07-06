#include "centergroup_index.h"

typedef struct {
    RotationBasis symmetries;
} CGIndexData;

CLArgumentList * centergroup_index_default_arguments() {
    return cl_argument_list_new();
}

int centergroup_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData) {    
    CGIndexData * data = (CGIndexData *)malloc(sizeof(CGIndexData));
    data->symmetries = params.symmetries;
    
    *userData = data;
    return 1;
}

int centergroup_index_load(HSParameters params, FILE * fp, void ** userData) {
    CuboidDimensions dims;
    if (!load_cuboid_dimensions(&dims, fp)) {
        return 0;
    }
    if (!cuboid_dimensions_equal(dims, params.symmetries.dims)) {
        return 0;
    }
    
    CGIndexData * data = (CGIndexData *)malloc(sizeof(CGIndexData));
    data->symmetries = params.symmetries;
    
    *userData = data;
    return 1;
}

void centergroup_index_save(void * userData, FILE * fp) {
    // nothing to be done, man.
    CGIndexData * data = (CGIndexData *)userData;
    save_cuboid_dimensions(data->symmetries.dims, fp);
}

int centergroup_index_supports_dimensions(void * userData, CuboidDimensions dims) {
    CGIndexData * data = (CGIndexData *)userData;
    return cuboid_dimensions_equal(dims, data->symmetries.dims);
}

int centergroup_index_data_size(void * userData) {
    CGIndexData * data = (CGIndexData *)userData;    
    
    CuboidDimensions dims = data->symmetries.dims;
    int numCenters = 0;
    numCenters += (dims.x - 2) * (dims.y - 2) * 2;
    numCenters += (dims.x - 2) * (dims.z - 2) * 2;
    numCenters += (dims.y - 2) * (dims.z - 2) * 2;
    if (numCenters % 2 != 0) return (numCenters / 2) + 1;
    else return numCenters / 2;
}

int centergroup_index_angle_count(void * userData) {
    return 1;
}

int centergroup_index_angles_are_equivalent(void * userData, int a1, int a2) {
    puts("error: centergroup_index_angles_are_equivalent() should never be called.");
    abort();
}

void centergroup_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle) {
    int indicesForCenters[6], currentIndex = 0;
    bzero(indicesForCenters, sizeof(int) * 6);
    bzero(out, centergroup_index_data_size(userData));
    int i, centerCount = cuboid_count_centers(cb);
    for (i = 0; i < centerCount; i++) {
        CuboidCenter c = cb->centers[i];
        int byteIndex = i / 2;
        int bitIndex = (i % 2 == 0 ? 0 : 4);
        int index = 0;
        if (indicesForCenters[c.side - 1] != 0) {
            index = indicesForCenters[c.side - 1];
        } else {
            currentIndex++;
            indicesForCenters[c.side - 1] = currentIndex;
            index = currentIndex;
        }
        uint8_t mask = (index << bitIndex);
        out[byteIndex] |= mask;
    }
}

void centergroup_index_completed(void * userData) {
    free(userData);
}

RotationBasis centergroup_index_data_symmetries(void * userData) {
    CGIndexData * data = (CGIndexData *)userData;
    return data->symmetries;
}
