#include "rotation_group.h"

static void _generate_from_basis(RotationGroup * group, RotationBasis basis);
static void _recursive_generate_basis(RotationGroup * group, Cuboid * soFar,
                                      Cuboid ** rotations, int count, int depth);
static Cuboid * _create_rotation(CuboidDimensions dims, CuboidMovesAxis axis, int power);

static int _rotation_group_closest_index(const RotationGroup * group, const Cuboid * c);

RotationBasis rotation_basis_standard(CuboidDimensions dims) {
    RotationBasis basis;
    basis.dims = dims;
    if (cuboid_validate_quarter_turn(dims, CuboidMovesAxisX)) {
        basis.xPower = 1;
    } else basis.xPower = 2;
    if (cuboid_validate_quarter_turn(dims, CuboidMovesAxisY)) {
        basis.yPower = 1;
    } else basis.yPower = 2;
    if (cuboid_validate_quarter_turn(dims, CuboidMovesAxisZ)) {
        basis.zPower = 1;
    } else basis.zPower = 2;
    return basis;
}

int rotation_basis_is_subset(RotationBasis general, RotationBasis subset) {
    assert(cuboid_dimensions_equal(general.dims, subset.dims));
    if (general.xPower != 0)
        if (subset.xPower % general.xPower) return 0;    
    if (general.yPower != 0)
        if (subset.yPower % general.yPower) return 0;
    if (general.zPower != 0)
        if (subset.zPower % general.zPower) return 0;
    return 1;
}

RotationGroup * rotation_group_create(CuboidDimensions dims) {
    RotationGroup * group = (RotationGroup *)malloc(sizeof(RotationGroup));
    bzero(group, sizeof(RotationGroup));
    group->retainCount = 1;
    group->dims = dims;
    return group;
}

RotationGroup * rotation_group_create_basis(RotationBasis basis) {
    RotationBasis standard = rotation_basis_standard(basis.dims);
    assert(rotation_basis_is_subset(standard, basis));
    
    RotationGroup * group = rotation_group_create(basis.dims);
    _generate_from_basis(group, basis);
    
    return group;
}

void rotation_group_release(RotationGroup * group) {
    group->retainCount--;
    if (group->retainCount == 0) {
        int i;
        for (i = 0; i < group->count; i++) {
            cuboid_free(group->cuboids[i]);
        }
        if (group->cuboids) free(group->cuboids);
        free(group);
    }
}

void rotation_group_retain(RotationGroup * group) {
    group->retainCount++;
}

/*************
 * Accessing *
 *************/

int rotation_group_contains(const RotationGroup * group, const Cuboid * cb) {
    int guess = _rotation_group_closest_index(group, cb);
    if (guess >= group->count) return 0;
    Cuboid * test = group->cuboids[guess];
    if (cuboid_light_comparison(test, cb) == 0) return 1;
    return 0;
}

int rotation_group_count(const RotationGroup * group) {
    return group->count;
}

Cuboid * rotation_group_get(const RotationGroup * group, int index) {
    assert(index >= 0 && index < group->count);
    return group->cuboids[index];
}

void rotation_group_add(RotationGroup * group, Cuboid * cb) {
    // figure out where we will insert the Cuboid
    int insertIndex = _rotation_group_closest_index(group, cb);
    if (insertIndex < group->count) {
        Cuboid * testCb = group->cuboids[insertIndex];
        int result = cuboid_light_comparison(cb, testCb);
        assert(result != 0);
        if (result > 0) {
            insertIndex++;
        }
    }
    
    // allocate a bigger cuboid buffer
    if (!group->cuboids) {
        group->cuboids = (Cuboid **)malloc(sizeof(Cuboid *));
    } else {
        int newSize = sizeof(Cuboid *) * (group->count + 1);
        group->cuboids = (Cuboid **)realloc(group->cuboids, newSize);
    }
    
    // shift for the insert
    int copyCount = group->count - insertIndex;
    if (copyCount > 0) {
        Cuboid ** sourceStart = &group->cuboids[insertIndex];
        Cuboid ** destStart = &group->cuboids[insertIndex + 1];
        int copySize = sizeof(Cuboid *) * copyCount;
        memmove((void *)destStart, (void *)sourceStart, copySize);
    }
    
    group->cuboids[insertIndex] = cb;
    group->count++;
}

/**********************
 * Private generation *
 **********************/

static void _generate_from_basis(RotationGroup * group, RotationBasis basis) {
    int basisCount = 0, i;
    Cuboid * rotations[3];
    if (basis.xPower > 0) {
        rotations[basisCount] = _create_rotation(basis.dims, CuboidMovesAxisX,
                                                 basis.xPower);
        basisCount++;
    }
    if (basis.yPower > 0) {
        rotations[basisCount] = _create_rotation(basis.dims, CuboidMovesAxisY,
                                                 basis.yPower);
        basisCount++;
    }
    if (basis.zPower > 0) {
        rotations[basisCount] = _create_rotation(basis.dims, CuboidMovesAxisZ,
                                                 basis.zPower);
        basisCount++;
    }
    Cuboid * identity = cuboid_create(basis.dims);
    _recursive_generate_basis(group, identity, rotations, basisCount, 0);
    cuboid_free(identity);
    for (i = 0; i < basisCount; i++) {
        cuboid_free(rotations[i]);
    }
}

static void _recursive_generate_basis(RotationGroup * group, Cuboid * soFar,
                                      Cuboid ** rotations, int count, int depth) {
    if (count == depth) {
        if (!rotation_group_contains(group, soFar)) {
            rotation_group_add(group, cuboid_copy(soFar));
        }
        return;
    }
    Cuboid * workspace = cuboid_create(group->dims);
    int i;
    for (i = 0; i < 4; i++) {
        Cuboid * place = cuboid_power(rotations[depth], i);
        cuboid_multiply(workspace, place, soFar);
        cuboid_free(place);
        _recursive_generate_basis(group, workspace, rotations,
                                  count, depth + 1);
    }
    cuboid_free(workspace);
}

static Cuboid * _create_rotation(CuboidDimensions dims, CuboidMovesAxis axis, int power) {
    // who uses one letter variable names now, noobs?
    assert(axis < 3);
    const char * axes = "xyz";
    Algorithm * a = algorithm_new_rotation(axes[axis]);
    a->power = power;
    Cuboid * c = algorithm_to_cuboid(a, dims);
    algorithm_free(a);
    return c;
}

/****************************
 * Private cuboid searching *
 ****************************/

static int _rotation_group_closest_index(const RotationGroup * group, const Cuboid * c) {
    int low = -1, high = group->count;
    while (high - low > 1) {
        int test = (high + low) / 2;
        Cuboid * cb = group->cuboids[test];
        int comparison = cuboid_light_comparison(c, cb);
        if (comparison > 0) {
            low = test;
        } else if (comparison < 0) {
            high = test;
        } else {
            low = test;
            high = test;
            break;
        }
    }
    
    int idx = (high + low) / 2;
    if (high == low) return idx;
    
    if (idx < 0) idx = 0;
    else if (idx > group->count) idx--;
    
    return idx;
}
