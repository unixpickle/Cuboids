#include "corner_index.h"

typedef struct {
    uint8_t quartersAllowed[3];
} CIData;

CLArgumentList * corner_index_default_arguments() {
    return cl_argument_list_new();
}

int corner_index_initialize(HSRunParameters params, CLArgumentList * arguments, void ** userData) {
    CIData * data = (CIData *)malloc(sizeof(CIData));
    int i;
    for (i = 0; i < 3; i++) {
        data->quartersAllowed[i] = cuboid_validate_quarter_turn(params.dimensions, i);
    }
    *userData = data;
    return 1;
}

int corner_index_load(HSSaveParameters params, FILE * fp, void ** userData) {
    CIData * data = (CIData *)malloc(sizeof(CIData));
    int i;
    for (i = 0; i < 3; i++) {
        uint8_t flag;
        if (fread(&flag, 1, 1, fp) != 1) {
            free(data);
            return 0;
        }
        data->quartersAllowed[i] = flag;
    }
    *userData = data;
    return 1;
}

void corner_index_save(void * userData, FILE * fp) {
    CIData * data = (CIData *)userData;
    int i;
    for (i = 0; i < 3; i++) {
        fwrite(&data->quartersAllowed[i], 1, 1, fp);
    }
}

int corner_index_supports_dimensions(void * userData, CuboidDimensions dims) {
    CIData * data = (CIData *)userData;
    int i;
    for (i = 0; i < 3; i++) {
        uint8_t shouldBe = data->quartersAllowed[i];
        if (shouldBe != cuboid_validate_quarter_turn(dims, i)) {
            return 0;
        }
    }
    return 1;
}

int corner_index_data_size(void * userData) {
    return 8;
}

int corner_index_angle_count(void * userData) {
    return 1;
}

void corner_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle) {
    assert(angle == 0);
    int i;
    for (i = 0; i < 8; i++) {
        CuboidCorner c = cb->corners[i];
        out[i] = c.index | (c.symmetry << 4);
    }
}

void corner_index_completed(void * userData) {
    free(userData);
}
