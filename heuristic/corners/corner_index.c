#include "corner_index.h"

CLArgumentList * corner_index_default_arguments() {
    return cl_argument_list_new();
}

int corner_index_run(CLSearchParameters * params, CLArgumentList * arguments, void ** userData) {
    return 1;
}

int corner_index_load(CLSearchParameters * params, FILE * fp, void ** userData) {
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
}
