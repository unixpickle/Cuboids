#include "cuboid_moves.h"

static MoveMap _quarter_turn_move_map(CuboidMovesAxis axis, int offset);
static int _center_for_face_turn(CuboidMovesAxis axis, int offset);

static void _quarter_turn_corners(Cuboid * out, CuboidMovesAxis axis, int offset);
static void _quarter_turn_edges(Cuboid * out, CuboidMovesAxis axis, int offset);
static void _quarter_turn_centers(Cuboid * out, CuboidMovesAxis axis, int offset);

static int _quarter_center_size(Cuboid * cuboid, CuboidMovesAxis axis);
static int _quarter_center_point_to_index(int size, int x, int y);
static void _quarter_center_point_from_index(int size, int index, int * x, int * y);
static void _quarter_rotate_center_point(int face, int * x, int * y);

int cuboid_validate_quarter_turn(CuboidDimensions dimensions,
                                 CuboidMovesAxis axis) {
    if (axis == CuboidMovesAxisX) {
        return (dimensions.y == dimensions.z);
    } else if (axis == CuboidMovesAxisY) {
        return (dimensions.x == dimensions.z);
    } else {
        return (dimensions.x == dimensions.y);
    }
}

Cuboid * cuboid_generate_quarter_face_turn(CuboidDimensions dimensions,
                                           CuboidMovesAxis axis,
                                           int offset) {
    assert(cuboid_validate_quarter_turn(dimensions, axis));
    assert(offset == -1 || offset == 1);
    Cuboid * out = cuboid_create(dimensions);
    _quarter_turn_corners(out, axis, offset);
    if (cuboid_count_edges(out) > 0) {
        _quarter_turn_edges(out, axis, offset);
    }
    if (cuboid_count_centers(out) > 0) {
        _quarter_turn_centers(out, axis, offset);
    }
    return out;
}

Cuboid * cuboid_generate_quarter_turn(CuboidDimensions dimensions,
                                      CuboidMovesAxis axis,
                                      int sliceOffset) {
    return NULL;                                      
}

/***********
 * PRIVATE *
 ***********/

static MoveMap _quarter_turn_move_map(CuboidMovesAxis axis, int offset) {
    int indexes[3][2] = {{0, 1}, {3, 2}, {5, 4}};
    int index = indexes[axis][(offset + 1) / 2];
    return MovesTable[index];
}

static int _center_for_face_turn(CuboidMovesAxis axis, int offset) {
    int indexes[3][2] = {{6, 5}, {4, 3}, {2, 1}};
    return indexes[axis][(offset + 1) / 2];
}

static void _quarter_turn_corners(Cuboid * out, CuboidMovesAxis axis, int offset) {
    int i;
    MoveMap map = _quarter_turn_move_map(axis, offset);
    for (i = 0; i < 4; i++) {
        int cornerIndex = map.corners[i];
        CuboidCorner corner;
        corner.symmetry = map.symmetry;
        corner.index = map.cornerPieces[i];
        out->corners[cornerIndex] = corner;
    }
}

static void _quarter_turn_edges(Cuboid * out, CuboidMovesAxis axis, int offset) {
    // this method assumes that all dedges on the face have the same number
    // of edges; this is a safe assumption because the face has been
    // validated for a quarter turn.
    
    // some notes for those who are getting confused (i.e. me):
    // - dedgeIndex is the physical slot, dedgePiece is the physical piece
    // - j is the physical slot, edgePiece is the physical piece
    // this is extra confusing because an edge has two coordinates!
    
    int i;
    MoveMap map = _quarter_turn_move_map(axis, offset);
    for (i = 0; i < 4; i++) {
        int dedgeIndex = map.dedges[i];
        int dedgePiece = map.dedgePieces[i];
        int j, dedgeSize = cuboid_count_edges_for_dedge(out, dedgeIndex);
        // place each edge
        for (j = 0; j < dedgeSize; j++) {
            int edgePiece = map.dedgesReorder[i] ? dedgeSize - j - 1 : j;
            int offset = cuboid_edge_index(out, dedgeIndex, j);
            CuboidEdge edge;
            edge.edgeIndex = edgePiece;
            edge.dedgeIndex = dedgePiece;
            edge.symmetry = map.symmetry;
            out->edges[offset] = edge;
        }
    }
}

static void _quarter_turn_centers(Cuboid * out, CuboidMovesAxis axis, int offset) {
    // some clarity for those who are confused:
    // this function goes through each center piece on the identity,
    // rotates it, and then writes it to its new position on the output.
    
    int face = _center_for_face_turn(axis, offset);
    int centerSize = _quarter_center_size(out, axis);
    int i;
    for (i = 0; i < centerSize * centerSize; i++) {
        int x, y;
        _quarter_center_point_from_index(centerSize, i, &x, &y);
        _quarter_rotate_center_point(face, &x, &y);
        int destIndex = _quarter_center_point_to_index(centerSize, x, y);
        
        int centerIndex = cuboid_center_index(out, face, destIndex);
        CuboidCenter c;
        c.side = face;
        c.index = i;
        out->centers[centerIndex] = c;
    }
}

// center specific logic, trig, etc.

static int _quarter_center_size(Cuboid * cuboid, CuboidMovesAxis axis) {
    // this method assumes that the turn has already been validated
    if (axis != CuboidMovesAxisX) return cuboid->dimensions.x - 2;
    return cuboid->dimensions.y - 2;
}

static int _quarter_center_point_to_index(int size, int x, int y) {
    y *= -1;
    x += size / 2;
    y += size / 2;
    return (y * size) + x;
}

static void _quarter_center_point_from_index(int size, int index, int * x, int * y) {
    *y = index / size;
    *x = index % size;
    *y -= size / 2;
    *x -= size / 2;
    *y *= -1;
}

static void _quarter_rotate_center_point(int face, int * x, int * y) {
    // this is very, very, very basic trig.
    int clockwise = 1, tmpX = *y, tmpY = *x;
    if (face == 2 || face == 5 || face == 4) clockwise = 0;
    if (clockwise) {
        *x = tmpY;
        *y = -tmpX;
    } else {
        *x = -tmpY;
        *y = tmpX;
    }
}

