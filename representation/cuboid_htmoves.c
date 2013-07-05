#include "cuboid_htmoves.h"

static void _ht_face_turn_corners(Cuboid * out, CuboidMovesAxis axis, int offset);
static void _ht_face_turn_edges(Cuboid * out, CuboidMovesAxis axis, int offset);
static void _ht_face_turn_centers(Cuboid * out, CuboidMovesAxis axis, int offset);

static void _ht_slice_centers(Cuboid * out, CuboidMovesAxis axis, int layer);
static void _ht_slice_edges(Cuboid * out, CuboidMovesAxis axis, int layer);

Cuboid * cuboid_half_face_turn(CuboidDimensions dims, CuboidMovesAxis axis, int offset) {
    assert(offset == -1 || offset == 1);
    Cuboid * out = cuboid_create(dims);
    
    if (out->corners) _ht_face_turn_corners(out, axis, offset);
    if (out->edges) _ht_face_turn_edges(out, axis, offset);
    if (out->centers) _ht_face_turn_centers(out, axis, offset);
    
    return out;
}

Cuboid * cuboid_half_slice(CuboidDimensions dims, CuboidMovesAxis axis, int layer) {
    // confirm that such a slice is possible
    int edgeSize = 0;
    if (axis == CuboidMovesAxisX) edgeSize = dims.x - 2;
    if (axis == CuboidMovesAxisY) edgeSize = dims.y - 2;
    if (axis == CuboidMovesAxisZ) edgeSize = dims.z - 2;
    assert(layer >= 0 && layer < edgeSize);
    
    Cuboid * out = cuboid_create(dims);
    _ht_slice_centers(out, axis, layer);
    _ht_slice_edges(out, axis, layer);
    
    return out;
}

/**************************
 * Permorming a face turn *
 **************************/

static void _ht_face_turn_corners(Cuboid * out, CuboidMovesAxis axis, int offset) {
    FaceMap map = cuboid_moves_face_map(axis, offset);
    int i;
    for (i = 0; i < 4; i++) {
        uint8_t sourceIndex = (i < 2 ? i + 2 : i - 2);
        uint8_t sourceCorner = map.corners[sourceIndex];
        uint8_t destCorner = map.corners[i];
        CuboidCorner corner;
        corner.index = sourceCorner;
        corner.symmetry = 0;
        out->corners[destCorner] = corner;
    }
}

static void _ht_face_turn_edges(Cuboid * out, CuboidMovesAxis axis, int offset) {
    FaceMap map = cuboid_moves_face_map(axis, offset);
    int i;
    for (i = 0; i < 4; i++) {
        uint8_t sourceIndex = (i < 2 ? i + 2 : i - 2);
        uint8_t sourceDedge = map.dedges[sourceIndex];
        uint8_t destDedge = map.dedges[i];
        
        uint8_t edgeCount = cuboid_count_edges_for_dedge(out, destDedge);
        uint8_t checkCount = cuboid_count_edges_for_dedge(out, sourceDedge);
        assert(edgeCount == checkCount);
        
        int j;
        for (j = 0; j < edgeCount; j++) {
            int dest = cuboid_edge_index(out, destDedge, edgeCount - j - 1);
            CuboidEdge edge;
            edge.dedgeIndex = sourceDedge;
            edge.edgeIndex = j;
            edge.symmetry = 0;
            out->edges[dest] = edge;
        }
    }
}

static void _ht_face_turn_centers(Cuboid * out, CuboidMovesAxis axis, int offset) {
    int face = cuboid_face_for_face_turn(axis, offset);
    int count = cuboid_count_centers_for_face(out, face);
    int i;
    for (i = 0; i < count; i++) {
        // 180 degree rotations are essentially an index complement
        int sourceIndex = count - i - 1;
        
        int destIndex = cuboid_center_index(out, face, i);
        CuboidCenter center;
        center.side = face;
        center.index = sourceIndex;
        out->centers[destIndex] = center;
    }
}

/**********************
 * Performing a slice *
 **********************/

static void _ht_slice_centers(Cuboid * out, CuboidMovesAxis axis, int layer) {
    SliceMap map = cuboid_moves_slice_map(axis);
    int i, j;
    for (i = 0; i < 4; i++) {
        int oppSideIndex = i < 2 ? i + 2 : i - 2;
        int lineLength = cuboid_slice_center_line_length(out->dimensions, map, i);
        for (j = 0; j < lineLength; j++) {
            int myIndex = cuboid_slice_center_line_index(out->dimensions, map,
                                                         i, layer, j);
            int oppIndex = cuboid_slice_center_line_index(out->dimensions, map,
                                                          oppSideIndex, layer, j);
            int writeIndex = cuboid_center_index(out, map.centers[i], myIndex);
            CuboidCenter c;
            c.side = map.centers[oppSideIndex];
            c.index = oppIndex;
            out->centers[writeIndex] = c;
        }
    }
}

static void _ht_slice_edges(Cuboid * out, CuboidMovesAxis axis, int layer) {
    SliceMap map = cuboid_moves_slice_map(axis);
    int i;
    for (i = 0; i < 4; i++) {
        int dedge = map.dedges[i];
        int oppDedge = map.dedges[i < 2 ? i + 2 : i - 2];
        int destIndex = cuboid_edge_index(out, dedge, layer);
        CuboidEdge edge;
        edge.dedgeIndex = oppDedge;
        edge.edgeIndex = layer;
        edge.symmetry = 0;
        out->edges[destIndex] = edge;
    }
}
