#include "cuboid_htmoves.h"

static void _ht_face_turn_corners(Cuboid * out, CuboidMovesAxis axis, int offset);
static void _ht_face_turn_edges(Cuboid * out, CuboidMovesAxis axis, int offset);
static void _ht_face_turn_centers(Cuboid * out, CuboidMovesAxis axis, int offset);

Cuboid * cuboid_half_face_turn(CuboidDimensions dims, CuboidMovesAxis axis, int offset) {
    assert(offset == -1 || offset == 1);
    Cuboid * out = cuboid_create(dims);
    
    if (out->corners) _ht_face_turn_corners(out, axis, offset);
    if (out->edges) _ht_face_turn_edges(out, axis, offset);
    if (out->centers) _ht_face_turn_centers(out, axis, offset);
    
    return out;
}

Cuboid * cuboid_half_slice(CuboidDimensions dims, CuboidMovesAxis axis, int layer) {
    // TODO: future alex
    
    return NULL;
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
    int i, w, h;
    cuboid_center_dimensions(out->dimensions, face, &w, &h);
    for (i = 0; i < count; i++) {
        int sourceX, sourceY;
        ht_center_point_from_index(w, h, i, &sourceX, &sourceY);
        ht_rotate_center_point(&sourceX, &sourceY);
        int sourceIndex = ht_center_point_to_index(w, h, sourceX, sourceY);
        
        int destIndex = cuboid_center_index(out, face, i);
        CuboidCenter center;
        center.side = face;
        center.index = sourceIndex;
        out->centers[destIndex] = center;
    }
}
