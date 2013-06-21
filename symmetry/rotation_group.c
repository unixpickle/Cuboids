#include "rotation_group.h"

static int _cuboid_light_comparison(const Cuboid * c1, const Cuboid * c2);
static int _rotation_group_closest_index(RotationGroup * group, Cuboid * c);

RotationBasis rotation_group_standard_basis(CuboidDimensions dims) {
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
    if (subset.xPower % general.xPower) return 0;
    if (subset.yPower % general.yPower) return 0;
    if (subset.zPower % general.zPower) return 0;
    return 1;
}

RotationGroup * rotation_group_create(RotationBasis basis) {
    RotationGroup * group = (RotationGroup *)malloc(sizeof(RotationGroup));
    bzero(group, sizeof(RotationGroup));
    group->retainCount = 1;
    group->dims = basis.dims;
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

int rotation_group_contains(RotationGroup * group, Cuboid * cb) {
    int guess = _rotation_group_closest_index(group, cb);
    Cuboid * test = group->cuboids[guess];
    if (_cuboid_light_comparison(test, cb) == 0) return 1;
    return 0;
}

void rotation_group_add(RotationGroup * group, Cuboid * cb) {
    if (group->count == 0) {
        group->cuboids = (Cuboid **)malloc(sizeof(Cuboid *));
    } else {
        int newSize = (group->count + 1) * sizeof(Cuboid *);
        group->cuboids = (Cuboid **)realloc(group->cuboids, newSize);
    }
    group->cuboids[group->count] = cb;
    group->count++;
}

/****************************
 * Private cuboid searching *
 ****************************/

static int _cuboid_light_comparison(const Cuboid * c1, const Cuboid * c2) {
    int i;
    int edgeCount = cuboid_count_edges(c1);
    int centerCount = cuboid_count_centers(c1);
    for (i = 0; i < edgeCount; i++) {
        CuboidEdge e1 = c1->edges[i];
        CuboidEdge e2 = c2->edges[i];
        if (e1.dedgeIndex < e2.dedgeIndex) return -1;
        if (e1.dedgeIndex > e2.dedgeIndex) return 1;
        if (e1.symmetry < e2.symmetry) return -1;
        if (e1.symmetry > e2.symmetry) return 1;
    }
    for (i = 0; i < centerCount; i++) {
        CuboidCenter ce1 = c1->centers[i];
        CuboidCenter ce2 = c2->centers[i];
        if (ce1.side < ce2.side) return -1;
        if (ce1.side > ce2.side) return 1;
    }
    for (i = 0; i < 8; i++) {
        CuboidCorner co1 = c1->corners[i];
        CuboidCorner co2 = c2->corners[i];
        if (co1.index < co2.index) return -1;
        if (co1.index > co2.index) return 1;
        if (co1.symmetry < co2.symmetry) return -1;
        if (co1.symmetry > co2.symmetry) return 1;
    }
    return 0;
}

static int _rotation_group_closest_index(RotationGroup * group, Cuboid * c) {
    int low = -1, high = group->count;
    while (high - low > 1) {
        int test = (high + low) / 2;
        Cuboid * cb = group->cuboids[test];
        int comparison = _cuboid_light_comparison(c, cb);
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
    else if (idx >= group->count) idx = group->count - 1;
    
    return idx;
}
