#include "cuboid.h"

static Cuboid * _base_cuboid_for_algorithm(Algorithm * algorithm, CuboidDimensions dims);
static Cuboid * _base_cuboid_for_slice(Algorithm * slice, CuboidDimensions dims);
static Cuboid * _base_cuboid_for_rotation(Algorithm * rotation, CuboidDimensions dims);
static Cuboid * _base_cuboid_for_wide_turn(Algorithm * turn, CuboidDimensions dims);
static Cuboid * _base_cuboid_for_container(Algorithm * container, CuboidDimensions dims);

static CuboidMovesAxis _axis_for_slice(Algorithm * algo);
static CuboidMovesAxis _axis_for_wide_turn(Algorithm * algo);
static CuboidMovesAxis _axis_for_cube_rotation(Algorithm * algo);
static int _axis_side_length(CuboidDimensions dim, CuboidMovesAxis axis);
static void _axis_normal_face_size(CuboidDimensions dim, CuboidMovesAxis axis,
                                   int * w, int * h);
static int _axis_validate_turn(CuboidDimensions dim, CuboidMovesAxis axis,
                               Algorithm * a);

// slices
static Cuboid * _slice_create_half(CuboidMovesAxis axis, CuboidDimensions dims);
static Cuboid * _slice_create_quarter(CuboidMovesAxis axis, CuboidDimensions dims);

// rotations
static Cuboid * _rotation_create_half(CuboidMovesAxis axis, CuboidDimensions dims);
static Cuboid * _rotation_create_quarter(CuboidMovesAxis axis, CuboidDimensions dims);

// wide turns
static Cuboid * _wide_turn_create_half(Algorithm * a, CuboidMovesAxis axis,
                                       CuboidDimensions dims);
static Cuboid * _wide_turn_create_quarter(Algorithm * a, CuboidMovesAxis axis,
                                          CuboidDimensions dims);

// returns -1 for left, back, down, 1 otherwise
int _wide_turn_face_offset(char face);
// returns 0 for left, up, front, 1 otherwise
int _wide_turn_edge_offset(char face);
// returns 1 for yes
int _wide_turn_should_invert(char face);

Cuboid * algorithm_to_cuboid(Algorithm * algorithm, CuboidDimensions dims) {
    assert(dims.x >= 2 && dims.y >= 2 && dims.z >= 2);
    
    Cuboid * base = _base_cuboid_for_algorithm(algorithm, dims);
    if (!base) return NULL;
    if (algorithm->inverseFlag) {
        Cuboid * inv = cuboid_inverse(base);
        cuboid_free(base);
        base = inv;
    }
    Cuboid * result = cuboid_copy(base);
    int i;
    int repeatCount = algorithm->power / (algorithm->power % 2 == 0 ? 2 : 1);
    for (i = 1; i < repeatCount; i++) {
        cuboid_multiply_to(base, result);
    }
    cuboid_free(base);
    return result;
}

static Cuboid * _base_cuboid_for_algorithm(Algorithm * algorithm, CuboidDimensions dims) {
    switch (algorithm->type) {
        case AlgorithmTypeSlice:
            return _base_cuboid_for_slice(algorithm, dims);
            break;
        case AlgorithmTypeRotation:
            return _base_cuboid_for_rotation(algorithm, dims);
            break;
        case AlgorithmTypeWideTurn:
            return _base_cuboid_for_wide_turn(algorithm, dims);
            break;
        default:
            return _base_cuboid_for_container(algorithm, dims);
            break;
    }
}

static Cuboid * _base_cuboid_for_slice(Algorithm * slice, CuboidDimensions dims) {
    CuboidMovesAxis axis = _axis_for_slice(slice);
    int sideLen = _axis_side_length(dims, axis);
    if (sideLen <= 2) return NULL;
    if (!_axis_validate_turn(dims, axis, slice)) return NULL;
    
    if (slice->power % 2 == 0) {
        return _slice_create_half(axis, dims);
    } else {
        return _slice_create_quarter(axis, dims);
    }
}

static Cuboid * _base_cuboid_for_rotation(Algorithm * rotation, CuboidDimensions dims) {
    CuboidMovesAxis axis = _axis_for_cube_rotation(rotation);
    if (!_axis_validate_turn(dims, axis, rotation)) return NULL;
    
    if (rotation->power % 2 == 0) {
        return _rotation_create_half(axis, dims);
    } else {
        return _rotation_create_quarter(axis, dims);
    }
}

static Cuboid * _base_cuboid_for_wide_turn(Algorithm * turn, CuboidDimensions dims) {
    CuboidMovesAxis axis = _axis_for_wide_turn(turn);
    int sideLen = _axis_side_length(dims, axis);
    if (turn->contents.wideTurn.numLayers >= sideLen) return NULL;
    if (!_axis_validate_turn(dims, axis, turn)) return NULL;
    
    if (turn->power % 2 == 0) {
        return _wide_turn_create_half(turn, axis, dims);
    } else {
        return _wide_turn_create_quarter(turn, axis, dims);
    }
}

static Cuboid * _base_cuboid_for_container(Algorithm * container, CuboidDimensions dims) {
    int count = algorithm_container_count(container);
    int i;
    Cuboid * result = cuboid_create(dims);
    Cuboid * temp = cuboid_copy(result);
    for (i = 0; i < count; i++) {
        Algorithm * algo = algorithm_container_get(container, i);
        Cuboid * next = algorithm_to_cuboid(algo, dims);
        
        if (!next) {
            cuboid_free(result);
            cuboid_free(temp);
            return NULL;
        }
        
        cuboid_multiply(result, next, temp);
        cuboid_free(next);
        if (i + 1 < count) {
            cuboid_free(temp);
            temp = cuboid_copy(result);
        }
    }
    cuboid_free(temp);
    
    if (container->power % 2 == 0) {
        // square it
        temp = cuboid_copy(result);
        cuboid_multiply_to(temp, result);
        cuboid_free(temp);
    }
    
    return result;
}

/******************
 * Axis functions *
 ******************/

static CuboidMovesAxis _axis_for_slice(Algorithm * algo) {
    if (algo->contents.slice.layer == 'M') return CuboidMovesAxisX;
    if (algo->contents.slice.layer == 'E') return CuboidMovesAxisY;
    if (algo->contents.slice.layer == 'S') return CuboidMovesAxisZ;
    return -1;
}

static CuboidMovesAxis _axis_for_wide_turn(Algorithm * algo) {
    const struct {
        char face1, face2;
        CuboidMovesAxis axis;
    } AxesMap[] = {
        {'R', 'L', CuboidMovesAxisX},
        {'U', 'D', CuboidMovesAxisY},
        {'F', 'B', CuboidMovesAxisZ}
    };
    int i;
    char face = algo->contents.wideTurn.face;
    for (i = 0; i < 3; i++) {
        if (AxesMap[i].face1 == face || AxesMap[i].face2 == face) {
            return AxesMap[i].axis;
        }
    }
    return -1;
}

static CuboidMovesAxis _axis_for_cube_rotation(Algorithm * algo) {
    if (algo->contents.rotation.axis == 'x') {
        return CuboidMovesAxisX;
    }
    if (algo->contents.rotation.axis == 'y') {
        return CuboidMovesAxisY;
    }
    if (algo->contents.rotation.axis == 'z') {
        return CuboidMovesAxisZ;
    }
    return -1;
}

static int _axis_side_length(CuboidDimensions dim, CuboidMovesAxis axis) {
    assert(axis <= CuboidMovesAxisZ);
    if (axis == CuboidMovesAxisX) return dim.x;
    if (axis == CuboidMovesAxisY) return dim.y;
    if (axis == CuboidMovesAxisZ) return dim.z;
    return 0;
}

static void _axis_normal_face_size(CuboidDimensions dim, CuboidMovesAxis axis,
                                   int * w, int * h) {
    if (axis == CuboidMovesAxisX) {
        *h = dim.y;
        *w = dim.z;
    } else if (axis == CuboidMovesAxisY) {
        *h = dim.z;
        *w = dim.x;
    } else {
        *h = dim.y;
        *w = dim.x;
    }
}

static int _axis_validate_turn(CuboidDimensions dim, CuboidMovesAxis axis,
                               Algorithm * a) {
    int w, h;
    _axis_normal_face_size(dim, axis, &w, &h);
    if (w != h) {
        if (a->power % 2 != 0) {
            return 0;
        }
    }
    return 1;
}

/**********
 * Slices *
 **********/

static Cuboid * _slice_create_half(CuboidMovesAxis axis, CuboidDimensions dims) {
    Cuboid * basic;
    int sideLength = _axis_side_length(dims, axis);
    int lowerOffset = (sideLength - 3) / 2;
    basic = cuboid_half_slice(dims, axis, lowerOffset);
    if (sideLength % 2 == 0) {
        Cuboid * otherSlice = cuboid_half_slice(dims, axis, lowerOffset + 1);
        cuboid_multiply_to(otherSlice, basic);
        cuboid_free(otherSlice);
    }
    return basic;
}

static Cuboid * _slice_create_quarter(CuboidMovesAxis axis, CuboidDimensions dims) {
    Cuboid * basic;
    int sideLength = _axis_side_length(dims, axis);
    int lowerOffset = (sideLength - 3) / 2;
    basic = cuboid_quarter_slice(dims, axis, lowerOffset);
    if (sideLength % 2 == 0) {
        Cuboid * otherSlice = cuboid_quarter_slice(dims, axis, lowerOffset + 1);
        cuboid_multiply_to(otherSlice, basic);
        cuboid_free(otherSlice);
    }
    
    // invert E slices because they're backwards :'(
    if (axis == CuboidMovesAxisY) {
        Cuboid * inv = cuboid_inverse(basic);
        cuboid_free(basic);
        basic = inv;
    }
    return basic;
}

/*************
 * Rotations *
 *************/

static Cuboid * _rotation_create_half(CuboidMovesAxis axis, CuboidDimensions dims) {
    int i, sideLength = _axis_side_length(dims, axis);
    Cuboid * rotation = cuboid_create(dims);
    
    for (i = 1; i < sideLength - 1; i++) {
        Cuboid * slice = cuboid_half_slice(dims, axis, i - 1);
        cuboid_multiply_to(slice, rotation);
        cuboid_free(slice);
    }
    
    for (i = -1; i < 2; i++) {
        if (i == 0) continue;
        Cuboid * outerTurn = cuboid_half_face_turn(dims, axis, i);
        cuboid_multiply_to(outerTurn, rotation);
        cuboid_free(outerTurn);
    }
    
    return rotation;
}

static Cuboid * _rotation_create_quarter(CuboidMovesAxis axis, CuboidDimensions dims) {
    int i, sideLength = _axis_side_length(dims, axis);
    Cuboid * rotation = cuboid_create(dims);
    
    for (i = 1; i < sideLength - 1; i++) {
        Cuboid * slice = cuboid_quarter_slice(dims, axis, i - 1);
        cuboid_multiply_to(slice, rotation);
        cuboid_free(slice);
    }
    
    for (i = -1; i < 2; i++) {
        if (i == 0) continue;
        Cuboid * outerTurn = cuboid_quarter_face_turn(dims, axis, i);
        cuboid_multiply_to(outerTurn, rotation);
        cuboid_free(outerTurn);
    }
    
    // rotations go with R, not L...
    if (axis == CuboidMovesAxisX) {
        Cuboid * inv = cuboid_inverse(rotation);
        cuboid_free(rotation);
        rotation = inv;
    }
    
    return rotation;
}

/**************
 * Wide turns *
 **************/

static Cuboid * _wide_turn_create_half(Algorithm * a, CuboidMovesAxis axis,
                                       CuboidDimensions dims) {
    int sideLength = _axis_side_length(dims, axis);
    int faceOffset = _wide_turn_face_offset(a->contents.wideTurn.face);
    int edgeOffset = _wide_turn_edge_offset(a->contents.wideTurn.face);
    Cuboid * cuboid = cuboid_half_face_turn(dims, axis, faceOffset);
    
    int edgeIndex = (sideLength - 3) * edgeOffset;
    int edgeIncrement = edgeOffset == 0 ? 1 : -1;
    int i;
    for (i = 0; i < a->contents.wideTurn.numLayers - 1; i++) {
        Cuboid * slice = cuboid_half_slice(dims, axis, edgeIndex);
        cuboid_multiply_to(slice, cuboid);
        cuboid_free(slice);
        edgeIndex += edgeIncrement;
    }
    
    return cuboid;
}

static Cuboid * _wide_turn_create_quarter(Algorithm * a, CuboidMovesAxis axis,
                                          CuboidDimensions dims) {
    int sideLength = _axis_side_length(dims, axis);
    int faceOffset = _wide_turn_face_offset(a->contents.wideTurn.face);
    int edgeOffset = _wide_turn_edge_offset(a->contents.wideTurn.face);
    Cuboid * cuboid = cuboid_quarter_face_turn(dims, axis, faceOffset);
    
    int edgeIndex = (sideLength - 3) * edgeOffset;
    int edgeIncrement = edgeOffset == 0 ? 1 : -1;
    int i;
    for (i = 0; i < a->contents.wideTurn.numLayers - 1; i++) {
        Cuboid * slice = cuboid_quarter_slice(dims, axis, edgeIndex);
        cuboid_multiply_to(slice, cuboid);
        cuboid_free(slice);
        edgeIndex += edgeIncrement;
    }
    
    if (_wide_turn_should_invert(a->contents.wideTurn.face)) {
        Cuboid * inv = cuboid_inverse(cuboid);
        cuboid_free(cuboid);
        cuboid = inv;
    }
    
    return cuboid;
}

// returns -1 for left, back, down, 1 otherwise
int _wide_turn_face_offset(char face) {
    if (face == 'L' || face == 'B' || face == 'D') {
        return -1;
    }
    return 1;
}

// returns 0 for left, up, front, 1 otherwise
int _wide_turn_edge_offset(char face) {
    if (face == 'L' || face == 'F' || face == 'U') {
        return 0;
    }
    return 1;
}

// returns 1 for yes
int _wide_turn_should_invert(char face) {
    return (face == 'R' || face == 'D' || face == 'B');
}
