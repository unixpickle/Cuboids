#include "rotation_cosets.h"

static void _generate_triggers(RotationCosets * cosets, RotationGroup * g1,
                               RotationGroup * subG, int isLeftCoset);

RotationCosets * rotation_cosets_create(RotationGroup * general, RotationGroup * subgroup) {
    assert(cuboid_dimensions_equal(general->dims, subgroup->dims));
    assert(general->count >= subgroup->count);
    assert(general->count % subgroup->count == 0);
    RotationCosets * cosets = (RotationCosets *)malloc(sizeof(RotationCosets));
    cosets->triggers = rotation_group_create(general->dims);
    cosets->retainCount = 1;
    
    _generate_triggers(cosets, general, subgroup, 1);
    
    return cosets;
}

RotationCosets * rotation_cosets_create_right(RotationGroup * general,
                                              RotationGroup * subgroup) {
    assert(cuboid_dimensions_equal(general->dims, subgroup->dims));
    assert(general->count >= subgroup->count);
    assert(general->count % subgroup->count == 0);
    RotationCosets * cosets = (RotationCosets *)malloc(sizeof(RotationCosets));
    cosets->triggers = rotation_group_create(general->dims);
    cosets->retainCount = 1;

    _generate_triggers(cosets, general, subgroup, 0);

    return cosets;
}

void rotation_cosets_release(RotationCosets * cosets) {
    cosets->retainCount--;
    if (cosets->retainCount == 0) {
        rotation_group_release(cosets->triggers);
        free(cosets);
    }
}

void rotation_cosets_retain(RotationCosets * cosets) {
    cosets->retainCount++;
}

/*************
 * Accessing *
 *************/

int rotation_cosets_count(RotationCosets * cosets) {
    return rotation_group_count(cosets->triggers);
}

Cuboid * rotation_cosets_get_trigger(RotationCosets * cosets, int index) {
    return rotation_group_get(cosets->triggers, index);
}

/***********
 * Private *
 ***********/

static void _generate_triggers(RotationCosets * cosets, RotationGroup * g1,
                               RotationGroup * subG, int isLeftCoset) {
    RotationGroup * generated = rotation_group_create(g1->dims);
    Cuboid * temp = cuboid_create(g1->dims);
    
    int i, j;
    for (i = 0; i < rotation_group_count(g1); i++) {
        Cuboid * cb = rotation_group_get(g1, i);
        int isOriginal = 1;
        for (j = 0; j < rotation_group_count(subG); j++) {
            Cuboid * subCb = rotation_group_get(subG, j);
            if (isLeftCoset) cuboid_multiply(temp, cb, subCb);
            else cuboid_multiply(temp, subCb, cb);
            if (rotation_group_contains(generated, temp)) {
                isOriginal = 0;
                break;
            } else {
                rotation_group_add(generated, cuboid_copy(temp));
            }
        }
        if (isOriginal) {
            rotation_group_add(cosets->triggers, cuboid_copy(cb));
        }
    }
    
    assert(generated->count == g1->count);
    
    rotation_group_release(generated);
    cuboid_free(temp);
}
