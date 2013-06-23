#include "standardpl.h"

CLArgumentList * standardpl_default_arguments() {
    return cl_argument_list_new();
}

int standardpl_run(CLSearchParameters * params, CLArgumentList * list, void ** userData) {
    if (list->count > 0) {
        return 0;
    }
    
    RotationBasis basis = rotation_basis_standard(params->dimensions);
    RotationGroup * group = rotation_group_create_basis(basis);
    *userData = group;
    
    return 1;
}

int standardpl_resume(CLSearchParameters * params, FILE * fp, void ** userData) {
    RotationBasis basis = rotation_basis_standard(params->dimensions);
    RotationGroup * group = rotation_group_create_basis(basis);
    *userData = group;
    return 1;
}

void standardpl_save(void * data, FILE * fp) {
    // nothing to be done
}

void standardpl_completed(void * data) {
    RotationGroup * group = (RotationGroup *)data;
    rotation_group_release(group);
}

int standardpl_is_goal(void * data, const Cuboid * cb, Cuboid * shouldBeNull) {
    RotationGroup * group = (RotationGroup *)data;
    return rotation_group_contains(group, cb);
}
