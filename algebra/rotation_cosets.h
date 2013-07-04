#ifndef __ROTATION_COSETS_H__
#define __ROTATION_COSETS_H__

#include "rotation_group.h"

typedef struct {
    // the RotationGroup object in this case is used to
    // represent an array of rotations rather than a group.
    RotationGroup * triggers;
    int retainCount;
} RotationCosets;

// generates all of the left cosets of `subgroup` in `general`
RotationCosets * rotation_cosets_create(RotationGroup * general, RotationGroup * subgroup);
RotationCosets * rotation_cosets_create_right(RotationGroup * general,
                                              RotationGroup * subgroup);
void rotation_cosets_release(RotationCosets * cosets);
void rotation_cosets_retain(RotationCosets * cosets);

int rotation_cosets_count(RotationCosets * cosets);
Cuboid * rotation_cosets_get_trigger(RotationCosets * cosets, int index);

#endif
