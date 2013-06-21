#include "notation/cuboid.h"
#include <assert.h>

typedef struct {
    int retainCount;
    int count;
    Cuboid ** cuboids;
    CuboidDimensions dims;
} RotationGroup;

typedef struct {
    CuboidDimensions dims;
    int xPower;
    int yPower;
    int zPower;
} RotationBasis;

RotationBasis rotation_group_standard_basis(CuboidDimensions dims);
int rotation_basis_is_subset(RotationBasis general, RotationBasis subset);

RotationGroup * rotation_group_create(RotationBasis basis);
void rotation_group_release(RotationGroup * group);
void rotation_group_retain(RotationGroup * group);

// contains compares the appearance, not the transformation
int rotation_group_contains(RotationGroup * group, Cuboid * cb);
void rotation_group_add(RotationGroup * group, Cuboid * cb);
