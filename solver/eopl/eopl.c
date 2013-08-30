#include "eopl.h"

static RotationCosets * generate_eo_cosets(CuboidDimensions dims, int axis);
static int _eo_are_centers_solved(const Cuboid * cb);
static int _eo_are_edges_oriented(const Cuboid * cb, int axis);

CLArgumentList * eopl_default_arguments() {
    CLArgumentList * list = cl_argument_list_new();
    
    cl_argument_list_add(list, cl_argument_new_integer("axis", 2));
    cl_argument_list_add(list, cl_argument_new_flag("centers", 0));
    
    return list;
}

int eopl_run(CLSearchParameters * params, CLArgumentList * list, void ** userData) {
    EOPluginContext * context = (EOPluginContext *)malloc(sizeof(EOPluginContext));
    
    // generate the cosets
    CLArgument * argument;
    int idx = cl_argument_list_find(list, "axis");
    argument = cl_argument_list_get(list, idx);
    
    int axis = argument->contents.integer.value;
    context->axis = axis;
    context->cosets = generate_eo_cosets(params->dimensions, axis);
    
    // other options
    idx = cl_argument_list_find(list, "centers");
    argument = cl_argument_list_get(list, idx);
    context->solveCenters = argument->contents.flag.boolValue;
    
    *userData = context;
    return 1;
}

int eopl_resume(CLSearchParameters * params, FILE * fp, void ** userData) {
    uint8_t axisNumber = 0, centerFlag = 0;
    if (fread(&axisNumber, 1, 1, fp) != 1) return 0;
    if (fread(&centerFlag, 1, 1, fp) != 1) return 0;
    if (axisNumber > 2) return 0;
    if (centerFlag > 1) return 0; 
    
    EOPluginContext * context = (EOPluginContext *)malloc(sizeof(EOPluginContext));
    context->axis = axisNumber;
    context->solveCenters = centerFlag;
    context->cosets = generate_eo_cosets(params->dimensions, axisNumber);
    
    *userData = context;
    return 1;
}

void eopl_save(void * data, FILE * fp) {
    EOPluginContext * context = (EOPluginContext *)data;
    uint8_t axisNumber = context->axis;
    uint8_t solveCenters = context->solveCenters;
    fwrite(&axisNumber, 1, 1, fp);
    fwrite(&solveCenters, 1, 1, fp);
}

void eopl_completed(void * data) {
    EOPluginContext * context = (EOPluginContext *)data;
    rotation_cosets_release(context->cosets);
    free(context);
}

int eopl_is_goal(void * data, const Cuboid * cb, Cuboid * cache) {
    // apply all of the cosets...
    EOPluginContext * context = (EOPluginContext *)data;
    int i;
    for (i = 0; i < rotation_cosets_count(context->cosets); i++) {
        Cuboid * rotation = rotation_cosets_get_trigger(context->cosets, i);
        cuboid_multiply(cache, rotation, cb);
        if (context->solveCenters) {
            if (!_eo_are_centers_solved(cache)) continue;
        }
        if (_eo_are_edges_oriented(cache, context->axis)) {
            return 1;
        }
    }
    return 0;
}

/***********
 * Private *
 ***********/

static RotationCosets * generate_eo_cosets(CuboidDimensions dims, int axis) {
    RotationBasis basis = {dims, 2, 2, 2};
    RotationBasis standard = rotation_basis_standard(dims);
    
    RotationGroup * whole = rotation_group_create_basis(standard);
    RotationGroup * subGroup = rotation_group_create_basis(basis);
    
    RotationCosets * cosets = rotation_cosets_create(whole, subGroup);
    rotation_group_release(whole);
    rotation_group_release(subGroup);
    return cosets;
}

static int _eo_are_centers_solved(const Cuboid * cb) {
    int face, i;
    for (face = 1; face <= 6; face++) {
        int count = cuboid_count_centers_for_face(cb, face);
        int thisColor = -1;
        for (i = 0; i < count; i++) {
            int index = cuboid_center_index(cb, face, i);
            CuboidCenter c = cb->centers[index];
            if ((c.side - 1) / 2 != (face - 1) / 2) return 0;
            if (thisColor < 0) thisColor = c.side;
            else if (c.side != thisColor) return 0;
        }
    }
    return 1;
}

static int _eo_are_edges_oriented(const Cuboid * cb, int axis) {
    int i, j;
    for (i = 0; i < 12; i++) {
        int edgeCount = cuboid_count_edges_for_dedge(cb, i);
        for (j = 0; j < edgeCount; j++) {
            int index = cuboid_edge_index(cb, i, j);
            CuboidEdge edge = cb->edges[index];
            if (!cuboid_edge_orientation(edge, i, axis)) {
                return 0;
            }
        }
    }
    return 1;
}
