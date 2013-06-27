#include "dedge_index.h"

static int _process_dedge_flags(uint8_t * flagsOut, const char * str);
static void _find_and_copy_dedge(const Cuboid * cb, uint8_t * out, int dedgeIndex);

typedef struct {
    uint8_t dedgeFlags[12];
    CuboidDimensions dims;
} DedgeIndexData;

CLArgumentList * dedge_index_default_arguments() {
    CLArgumentList * list = cl_argument_list_new();
    cl_argument_list_add(list, cl_argument_new_string("dedges", "111111111111"));
    return list;
}

int dedge_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData) {
    int index = cl_argument_list_find(arguments, "dedges");
    if (index < 0) return 0;
    CLArgument * arg = cl_argument_list_get(arguments, index);
    
    DedgeIndexData * data = (DedgeIndexData *)malloc(sizeof(DedgeIndexData));
    data->dims = params.symmetries.dims;
    if (!_process_dedge_flags(data->dedgeFlags, arg->contents.string.value)) {
        free(data);
        return 0;
    }
    
    *userData = data;
    return 1;
}

int dedge_index_load(HSParameters params, FILE * fp, void ** userData) {
    uint16_t saved;
    if (!load_uint16(&saved, fp)) return 0;
    DedgeIndexData * data = (DedgeIndexData *)malloc(sizeof(DedgeIndexData));
    int i;
    for (i = 0; i < 12; i++) {
        data->dedgeFlags[i] = (saved >> i) & 1;
    }
    if (!load_cuboid_dimensions(&data->dims, fp)) {
        free(data);
        return 0;
    }
    if (!dedge_index_supports_dimensions(data, params.symmetries.dims)) {
        free(data);
        return 0;
    }
    *userData = data;
    return 1;
}

void dedge_index_save(void * userData, FILE * fp) {
    DedgeIndexData * data = (DedgeIndexData *)userData;
    uint16_t flags = 0;
    int i;
    for (i = 0; i < 12; i++) {
        if (data->dedgeFlags[i]) {
            flags |= (1 << i);
        }
    }
    save_uint16(flags, fp);
    save_cuboid_dimensions(data->dims, fp);
}

int dedge_index_supports_dimensions(void * userData, CuboidDimensions dims) {
    DedgeIndexData * data = (DedgeIndexData *)userData;
    return cuboid_dimensions_equal(dims, data->dims);
}

int dedge_index_data_size(void * userData) {
    DedgeIndexData * data = (DedgeIndexData *)userData;
    int xCount = data->dims.x - 2, yCount = data->dims.y - 2,
        zCount = data->dims.z - 2;
    int countPerDedge[12] = {xCount, yCount, xCount, yCount, zCount, zCount,
                             xCount, yCount, xCount, yCount, zCount, zCount};
    int i, edgeCount = 0;
    for (i = 0; i < 12; i++) {
        if (data->dedgeFlags[i]) {
            edgeCount += countPerDedge[i];
        }
    }
    return edgeCount * 2;
}

int dedge_index_angle_count(void * userData) {
    return 1;
}

int dedge_index_angles_are_equivalent(void * userData, int a1, int a2) {
    puts("error: dedge_index_angles_are_equivalent() should never be called.");
    abort();
}

void dedge_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle) {
    DedgeIndexData * data = (DedgeIndexData *)userData;
    int dataIndex = 0, dedge;
    for (dedge = 0; dedge < 12; dedge++) {
        if (!data->dedgeFlags[dedge]) continue;
        
        int edgeCount = cuboid_count_edges_for_dedge(cb, dedge);
        _find_and_copy_dedge(cb, &out[dataIndex], dedge);
        dataIndex += edgeCount * 2;
    }
}

void dedge_index_completed(void * userData) {
    DedgeIndexData * data = (DedgeIndexData *)userData;
    free(data);
}

/***********
 * Private *
 ***********/

static int _process_dedge_flags(uint8_t * flagsOut, const char * str) {
    if (strlen(str) != 12) return 0;
    int i;
    for (i = 0; i < 12; i++) {
        if (str[i] == '1') {
            flagsOut[i] = 1;
        } else if (str[i] == '0') {
            flagsOut[i] = 0;
        } else {
            return 0;
        }
    }
    return 1;
}

static void _find_and_copy_dedge(const Cuboid * cb, uint8_t * out, int dedgeIndex) {
    // find the physical dedge and write information about it's physical slot and
    // current symmetry to *out.
    int dedge, edge, gottenCount = 0;
    int expected = cuboid_count_edges_for_dedge(cb, dedgeIndex);
    
    for (dedge = 0; dedge < 12; dedge++) {
        int edgeCount = cuboid_count_edges_for_dedge(cb, dedge);
        for (edge = 0; edge < edgeCount; edge++) {
            int index = cuboid_edge_index(cb, dedge, edge);
            CuboidEdge e = cb->edges[index];
            if (e.dedgeIndex == dedgeIndex) {
                out[gottenCount * 2] = dedge | (e.symmetry << 4);
                out[1 + gottenCount * 2] = edge;
                gottenCount++;
                if (gottenCount >= expected) break;
            }
        }
        if (gottenCount >= expected) break;
    }
    assert(gottenCount == expected);
}
