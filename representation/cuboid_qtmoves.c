#include "cuboid_qtmoves.h"

static MoveMap _qt_move_map(CuboidMovesAxis axis, int offset);
static SliceMap _qt_slice_map(CuboidMovesAxis axis);
static int _qt_face_for_face_turn(CuboidMovesAxis axis, int offset);
static int _qt_face_side_length(CuboidDimensions dim, CuboidMovesAxis axis);
static void _qt_face_center_dimensions(CuboidDimensions dim, int face, int * w, int * h);

static void _qt_rotate_corners(Cuboid * out, CuboidMovesAxis axis, int offset);
static void _qt_rotate_edges(Cuboid * out, CuboidMovesAxis axis, int offset);
static void _qt_rotate_centers(Cuboid * out, CuboidMovesAxis axis, int offset);

static int _qt_slice_edge_index(CuboidDimensions dim,
                                CuboidMovesAxis axis,
                                int offset);
// gives the index of a center piece on a center line
static int _qt_center_line_index(CuboidDimensions dim, int face,
                                 int axis, int flipped,
                                 int edgeIndex, int indexInLine);

static void _qt_slice_edges(Cuboid * out, CuboidMovesAxis axis, int offset);
static void _qt_slice_center(Cuboid * out, CuboidMovesAxis axis, int offset); 

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
    _qt_rotate_corners(out, axis, offset);
    if (cuboid_count_edges(out) > 0) {
        _qt_rotate_edges(out, axis, offset);
    }
    if (cuboid_count_centers(out) > 0) {
        _qt_rotate_centers(out, axis, offset);
    }
    return out;
}

Cuboid * cuboid_generate_quarter_turn(CuboidDimensions dimensions,
                                      CuboidMovesAxis axis,
                                      int sliceOffset) {
    assert(cuboid_validate_quarter_turn(dimensions, axis));
    int face = _qt_face_for_face_turn(axis, sliceOffset);
    
    
    
    return NULL;
}

/***********
 * PRIVATE *
 ***********/

static MoveMap _qt_move_map(CuboidMovesAxis axis, int offset) {
    int indexes[3][2] = {{0, 1}, {3, 2}, {5, 4}};
    int index = indexes[axis][(offset + 1) / 2];
    return MovesTable[index];
}

static SliceMap _qt_slice_map(CuboidMovesAxis axis) {
    return SlicesTable[axis];
}

static int _qt_face_for_face_turn(CuboidMovesAxis axis, int offset) {
    int indexes[3][2] = {{6, 5}, {4, 3}, {2, 1}};
    return indexes[axis][(offset + 1) / 2];
}

static int _qt_face_side_length(CuboidDimensions dim, CuboidMovesAxis axis) {
    // this method assumes that the turn has already been validated
    if (axis != CuboidMovesAxisX) return dim.x;
    return dim.y;
}

static void _qt_face_center_dimensions(CuboidDimensions dim, int face, int * w, int * h) {
    if (face == 1 || face == 2) {
        *w = dim.x - 2;
        *h = dim.y - 2;
    } else if (face == 3 || face == 4) {
        *w = dim.x - 2;
        *h = dim.z - 2;
    } else {
        *w = dim.z - 2;
        *h = dim.y - 2;
    }
}

/**************
 * Face turns *
 **************/

static void _qt_rotate_corners(Cuboid * out, CuboidMovesAxis axis, int offset) {
    int i;
    MoveMap map = _qt_move_map(axis, offset);
    for (i = 0; i < 4; i++) {
        int cornerIndex = map.corners[i];
        CuboidCorner corner;
        corner.symmetry = map.symmetry;
        corner.index = map.cornerPieces[i];
        out->corners[cornerIndex] = corner;
    }
}

static void _qt_rotate_edges(Cuboid * out, CuboidMovesAxis axis, int offset) {
    // this method assumes that all dedges on the face have the same number
    // of edges; this is a safe assumption because the face has been
    // validated for a quarter turn.
    
    // some notes for those who are getting confused (i.e. me):
    // - dedgeIndex is the physical slot, dedgePiece is the physical piece
    // - j is the physical slot, edgePiece is the physical piece
    // this is extra confusing because an edge has two coordinates!
    
    int i;
    MoveMap map = _qt_move_map(axis, offset);
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

static void _qt_rotate_centers(Cuboid * out, CuboidMovesAxis axis, int offset) {
    // some clarity for those who are confused:
    // this function goes through each center piece on the identity,
    // rotates it, and then writes it to its new position on the output.
    
    int face = _qt_face_for_face_turn(axis, offset);
    int centerSize = _qt_face_side_length(out->dimensions, axis) - 2;
    int i;
    for (i = 0; i < centerSize * centerSize; i++) {
        int x, y;
        qt_center_point_from_index(centerSize, i, &x, &y);        
        qt_rotate_center_point(face, &x, &y);
        int destIndex = qt_center_point_to_index(centerSize, x, y);
        
        int centerIndex = cuboid_center_index(out, face, destIndex);
        CuboidCenter c;
        c.side = face;
        c.index = i;
        out->centers[centerIndex] = c;
    }
}

/**********
 * Slices *
 **********/

static int _qt_slice_edge_index(CuboidDimensions dim,
                                CuboidMovesAxis axis,
                                int offset) {
    return 0;
    int sideLength;
    if (axis == CuboidMovesAxisX) sideLength = out->dimensions.x;
    if (axis == CuboidMovesAxisY) sideLength = out->dimensions.y;
    if (axis == CuboidMovesAxisZ) sideLength = out->dimensions.z;
    int edgeIndex = 0;
    if (sideLength % 2 == 0) {
        if (offset < 0) edgeIndex = (sideLength / 2 - 1) + offset;
        else edgeIndex = (sideLength / 2 - 2) + offset;
    } else {
        edgeIndex = (sideLength - 2) / 2 + offset;
    }
    return edgeIndex;
}

static int _qt_center_line_index(CuboidDimensions dim, int face,
                                 int axis, int flipped,
                                 int edgeIndex, int indexInLine) {
    return 0;
    int w, h;
    _qt_face_center_dimensions(dim, face, &w, &h);
    if (axis == 0) {
        // horizontal line
        int effectiveEdgeIndex = flipped ? w - edgeIndex - 1 : edgeIndex;
        int offset = 
    }
}

static void _qt_slice_edges(Cuboid * out, CuboidMovesAxis axis, int offset) {
    int i, edge = _qt_slice_edge_index(out->dimensions, axis, offset);
    SliceMap map = _qt_slice_map(axis);
    for (i = 0; i < 4; i++) {
        uint8_t physicalSlot = map.dedges[i];
        uint8_t physicalDedge;
        if (i > 0) physicalDedge = map.dedges[i - 1];
        else physicalDedge = map.dedges[3];
        
        CuboidEdge edge;
        edge.edgeIndex = edge;
        edge.dedgeIndex = physicalDedge;
        edge.symmetry = map.symmetry;
        
        int offset = cuboid_edge_index(out, physicalSlot, edge);
        out->edges[offset] = edge;
    }
}

static void _qt_slice_center(Cuboid * out, CuboidMovesAxis axis, int offset) {
    int i, edgeIdx = _qt_slice_edge_index(out->dimensions, axis, offset);
    SliceMap map = _qt_slice_map(axis);
    for (i = 0; i < 4; i++) {
        uint8_t centerSlot = map.centers[i];
        uint8_t centerSource;
        
    }
}
