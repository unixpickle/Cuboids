#include "cco_index.h"

typedef struct {
    CEOContext * ceoContext;
    COContext * coContext;
} CCOContext;

CLArgumentList * cco_index_default_arguments() {
    return cl_argument_list_new();
}

int cco_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData) {
    CCOContext * data = (CCOContext *)malloc(sizeof(CCOContext));
    data->ceoContext = ceo_context_create(params.symmetries);
    data->coContext = co_context_create(params.symmetries);
    *userData = data;
    return 1;
}

int cco_index_load(HSParameters params, FILE * fp, void ** userData) {
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
    CCOContext * data = (CCOContext *)malloc(sizeof(CCOContext));
    data->coContext = co_context_create(symmetries);
    data->ceoContext = ceo_context_create(symmetries);
    *userData = data;
    return 1;
}

void cco_index_save(void * userData, FILE * fp) {
    CCOContext * context = (CCOContext *)userData;
    RotationBasis symmetries = context->coContext->symmetries;
    save_uint8(symmetries.xPower, fp);
    save_uint8(symmetries.yPower, fp);
    save_uint8(symmetries.zPower, fp);
    save_cuboid_dimensions(symmetries.dims, fp);
}

int cco_index_supports_dimensions(void * userData, CuboidDimensions dims) {
    CCOContext * context = (CCOContext *)userData;
    return cuboid_dimensions_equal(context->coContext->symmetries.dims, dims);
}

int cco_index_data_size(void * userData) {
    CCOContext * context = (CCOContext *)userData;
    return 2 + ceo_context_data_size(context->ceoContext);
}

int cco_index_angle_count(void * userData) {
    return 3;
}

int cco_index_angles_are_equivalent(void * userData, int a1, int a2) {
    CCOContext * context = (CCOContext *)userData;
    int compat1 = co_context_axis_compatibility(context->coContext, a1, a2);
    int compat2 = ceo_context_axis_compatibility(context->ceoContext, a1, a2);
    assert(compat1 == compat2);
    return compat1;
}

void cco_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle) {
    CCOContext * context = (CCOContext *)userData;
    uint16_t coData = co_context_get_data(context->coContext, cb, angle);
    bzero(out, cco_index_data_size(userData));
    ceo_context_get_data(context->ceoContext, cb, &out[2], angle);
    out[0] = coData & 0xff;
    out[1] = (coData >> 8) & 0xff;
}

void cco_index_completed(void * userData) {
    CCOContext * context = (CCOContext *)userData;
    co_context_free(context->coContext);
    ceo_context_free(context->ceoContext);
    free(context);
}

RotationBasis cco_index_data_symmetries(void * userData) {
    CCOContext * generalContext = (CCOContext *)userData;
    RotationBasis sym = generalContext->coContext->symmetries;
    RotationBasis basis = {sym.dims, 2, 2, 2};
    if (sym.xPower == 0) basis.xPower = 0;
    if (sym.yPower == 0) basis.yPower = 0;
    if (sym.zPower == 0) basis.zPower = 0;
    return basis;
}
