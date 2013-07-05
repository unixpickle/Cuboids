#include "eo_index.h"

CLArgumentList * eo_index_default_arguments() {
    return cl_argument_list_new();
}

int eo_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData) {
    *userData = eo_context_create(params.symmetries);
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
    EOContext * context = eo_context_create(symmetries);
    *userData = context;
    return 1;
}

void eo_index_save(void * userData, FILE * fp) {
    EOContext * context = (EOContext *)userData;
    save_uint8(context->symmetries.xPower, fp);
    save_uint8(context->symmetries.yPower, fp);
    save_uint8(context->symmetries.zPower, fp);
    save_cuboid_dimensions(context->symmetries.dims, fp);
}

int eo_index_supports_dimensions(void * userData, CuboidDimensions dims) {
    EOContext * context = (EOContext *)userData;
    return cuboid_dimensions_equal(context->symmetries.dims, dims);
}

int eo_index_data_size(void * userData) {
    EOContext * context = (EOContext *)userData;
    return eo_context_compact_data_length(context);
}

int eo_index_angle_count(void * userData) {
    return 3;
}

int eo_index_angles_are_equivalent(void * userData, int a1, int a2) {
    EOContext * context = (EOContext *)userData;
    return eo_context_axis_compatibility(context, a1, a2);
}

void eo_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle) {
    EOContext * context = (EOContext *)userData;
    eo_context_get_compact_data(context, cb, out, angle);
}

void eo_index_completed(void * userData) {
    EOContext * context = (EOContext *)userData;
    eo_context_free(context);
}

RotationBasis eo_index_data_symmetries(void * userData) {
    EOContext * context = (EOContext *)userData;
    RotationBasis basis = {context->symmetries.dims, 2, 2, 2};
    if (context->symmetries.xPower == 0) basis.xPower = 0;
    if (context->symmetries.yPower == 0) basis.yPower = 0;
    if (context->symmetries.zPower == 0) basis.zPower = 0;
    return basis;
    
}
