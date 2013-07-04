#include "dedgepair_index.h"

typedef struct {
    RotationBasis symmetries;
} DPIndexData;

CLArgumentList * dedgepair_index_default_arguments() {
    return cl_argument_list_new();
}

int dedgepair_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData) {    
    DPIndexData * data = (DPIndexData *)malloc(sizeof(DPIndexData));
    data->symmetries = params.symmetries;
    
    *userData = data;
    return 1;
}

int dedgepair_index_load(HSParameters params, FILE * fp, void ** userData) {
    CuboidDimensions dims;
    if (!load_cuboid_dimensions(&dims, fp)) {
        return 0;
    }
    if (!cuboid_dimensions_equal(dims, params.symmetries.dims)) {
        return 0;
    }
    
    DPIndexData * data = (DPIndexData *)malloc(sizeof(DPIndexData));
    data->symmetries = params.symmetries;
    
    *userData = data;
    return 1;
}

void dedgepair_index_save(void * userData, FILE * fp) {
    // nothing to be done, man.
    DPIndexData * data = (DPIndexData *)userData;
    save_cuboid_dimensions(data->symmetries.dims, fp);
}

int dedgepair_index_supports_dimensions(void * userData, CuboidDimensions dims) {
    DPIndexData * data = (DPIndexData *)userData;
    return cuboid_dimensions_equal(dims, data->symmetries.dims);
}

int dedgepair_index_data_size(void * userData) {
    DPIndexData * data = (DPIndexData *)userData;    
    
    CuboidDimensions dims = data->symmetries.dims;
    int numEdges = (dims.x - 2) * 4 + (dims.y - 2) * 4 + (dims.z - 2) * 4;
    if (numEdges % 2 != 0) return (numEdges / 2) + 1;
    else return numEdges / 2;
}

int dedgepair_index_angle_count(void * userData) {
    return 1;
}

int dedgepair_index_angles_are_equivalent(void * userData, int a1, int a2) {
    puts("error: dedgepair_index_angles_are_equivalent() should never be called.");
    abort();
}

void dedgepair_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle) {
    int indicesForDedges[12], currentIndex = 0;
    bzero(indicesForDedges, sizeof(int) * 12);
    bzero(out, dedgepair_index_data_size(userData));
    int i, edgeCount = cuboid_count_edges(cb);
    for (i = 0; i < edgeCount; i++) {
        CuboidEdge edge = cb->edges[i];
        int byteIndex = i / 2;
        int bitIndex = (i % 2 == 0 ? 0 : 4);
        int index = 0;
        if (indicesForDedges[edge.dedgeIndex] != 0) {
            index = indicesForDedges[edge.dedgeIndex];
        } else {
            currentIndex++;
            indicesForDedges[edge.dedgeIndex] = currentIndex;
            index = currentIndex;
        }
        uint8_t mask = (index << bitIndex);
        out[byteIndex] |= mask;
    }
}

void dedgepair_index_completed(void * userData) {
    DPIndexData * data = (DPIndexData *)userData;
    free(data);
}

RotationBasis dedgepair_index_data_symmetries(void * userData) {
    DPIndexData * data = (DPIndexData *)userData;
    return data->symmetries;
}
