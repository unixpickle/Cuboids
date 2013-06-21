#include "rotation_group.h"

typedef struct {
    // the RotationGroup object in this case is used to
    // represent an array of rotations rather than a group.
    RotationGroup * triggers;
    int retainCount;
} RotationCosets;

// generates all of the left cosets of `subgroup` in `general`
RotationCosets * rotation_cosets_create(RotationGroup * general, RotationGroup * subgroup);
void rotation_cosets_release(RotationCosets * cosets);
void rotation_cosets_retain(RotationCosets * cosets);
