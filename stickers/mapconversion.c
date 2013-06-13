#include "mapconversion.h"

// StickerMap -> Cuboid

static int _convert_stickermap_edges(Cuboid * cuboid, const StickerMap * sm);
static int _convert_stickermap_corners(Cuboid * cuboid, const StickerMap * sm);
static int _convert_stickermap_centers(Cuboid * cuboid, const StickerMap * sm);

// dealing with individual sticker indexes
static uint32_t _sm_edge_index(const StickerMap * sm,
                                int face, int dedgePos,
                                int edgeIndex, int flip);
static uint32_t _sm_corner_index(const StickerMap * sm,
                                 int face, int x, int y);
static uint32_t _sm_center_index(const StickerMap * sm,
                                 int face, int index);

void convert_sm_to_cb(Cuboid * cuboid, const StickerMap * map) {
    if (cuboid->edges) {
        _convert_stickermap_edges(cuboid, map);
    }
    if (cuboid->centers) {
        _convert_stickermap_centers(cuboid, map);
    }
    _convert_stickermap_corners(cuboid, map);
}

void convert_cb_to_sm(StickerMap * map, const Cuboid * cuboid) {
    abort();
}

Triple converter_sm_edge_stickers(const StickerMap * sm,
                                  int dedge, int edge) {
    Triple stickers = {0, 0, 0};
    
    DedgeMap map = DedgesTable[dedge];
    int i;
    for (i = 0; i < 2; i++) {
        uint8_t face = map.sides[i].face;
        uint8_t position = map.sides[i].position;
        uint8_t flipFlag = map.sides[i].flipFlag;
        uint32_t stickerIndex = _sm_edge_index(sm, face, position,
                                                edge, flipFlag);
        uint8_t sticker = sm->stickers[stickerIndex];
        if (face == 1 || face == 2) {
            stickers.z = sticker;
        } else if (face == 3 || face == 4) {
            stickers.y = sticker;
        } else stickers.x = sticker;
    }
    
    return stickers;
}

Triple converter_sm_corner_stickers(const StickerMap * sm,
                                    int corner) {
    uint8_t stickers[3];
    CornerMap map = CornersTable[corner];
    int i;
    for (i = 0; i < 3; i++) {
        int face = map.sides[i].face;
        int x = map.sides[i].x, y = map.sides[i].y;
        uint32_t stickerIndex = _sm_corner_index(sm, face, x, y);
        stickers[i] = sm->stickers[stickerIndex];
    }
    Triple t = {stickers[0], stickers[1], stickers[2]};
    return t;
}

int converter_dedge_for_triple(Triple t, int * symmetry) {
    uint8_t tVal[3] = {t.x, t.y, t.z};
    int i;
    for (i = 0; i < 12; i++) {
        int sym = symmetry3_operation_find(EdgePieces[i], tVal);
        if (sym >= 0) {
            if (symmetry) *symmetry = sym;
            return i;
        }
    }
    return -1;
}

int converter_corner_for_triple(Triple t, int * symmetry) {
    uint8_t tVal[3] = {t.x, t.y, t.z};
    int i;
    for (i = 0; i < 8; i++) {
        int sym = symmetry3_operation_find(CornerPieces[i], tVal);
        if (sym >= 0) {
            if (symmetry) *symmetry = sym;
            return i;
        }
    }
    return -1;
}

/************************
 * StickerMap -> Cuboid *
 ************************/

static int _convert_stickermap_edges(Cuboid * cuboid, const StickerMap * sm) {
    // read a triple for each edge, figure out which dedge it came from,
    // and slap that info into our Cuboid.
    int dedgeSlot, edgeSlot;
    for (dedgeSlot = 0; dedgeSlot < 12; dedgeSlot++) {
        int edgeCount = cuboid_count_edges_for_dedge(cuboid, dedgeSlot);
        for (edgeSlot = 0; edgeSlot < edgeCount; edgeSlot++) {
            Triple stickers = converter_sm_edge_stickers(sm, dedgeSlot, edgeSlot);
            int symmetry, dedgePiece;
            dedgePiece = converter_dedge_for_triple(stickers, &symmetry);
            if (dedgePiece < 0) return 0;
            
            int edgeAddr = cuboid_edge_index(cuboid, dedgeSlot, edgeSlot);
            CuboidEdge edge;
            edge.symmetry = symmetry;
            edge.dedgeIndex = dedgePiece;
            edge.edgeIndex = 0; // unknown and unimportant for now
            cuboid->edges[edgeAddr] = edge;
        }
    }
    return 1;
}

static int _convert_stickermap_corners(Cuboid * cuboid, const StickerMap * sm) {
    // read a triple for a corner, figure out which corner it came from,
    // and put that info into our Cuboid's corner data.
    int corner;
    for (corner = 0; corner < 8; corner++) {
        Triple stickers = converter_sm_corner_stickers(sm, corner);
        int symmetry, cornerPiece;
        cornerPiece = converter_corner_for_triple(stickers, &symmetry);
        if (cornerPiece < 0) return 0;
        
        CuboidCorner c;
        c.symmetry = symmetry;
        c.index = cornerPiece;
        cuboid->corners[corner] = c;
    }
    return 1;
}

static int _convert_stickermap_centers(Cuboid * cuboid, const StickerMap * sm) {
    int face, centerSlot;
    for (face = 1; face <= 6; face++) {
        int centerCount = cuboid_count_centers_for_face(cuboid, face);
        for (centerSlot = 0; centerSlot < centerCount; centerSlot++) {
            uint32_t stickerIndex = _sm_center_index(sm, face, centerSlot);
            uint8_t sticker = sm->stickers[stickerIndex];
            
            int centerAddr = cuboid_center_index(cuboid, face, centerSlot);
            CuboidCenter c;
            c.side = sticker;
            c.index = 0; // we couldn't possibly know this
            cuboid->centers[centerAddr] = c;
        }
    }
    return 1;
}

/**************************
 * General indexing tools *
 **************************/

static uint32_t _sm_edge_index(const StickerMap * sm,
                                int face, int dedgePos,
                                int edgeIndex, int flip) {
    int x, y, w, h;
    stickermap_dimensions_of_face(sm, face, &w, &h);
    if (dedgePos == 0 || dedgePos == 2) {
        // horizontal dedge
        y = (dedgePos == 0 ? 0 : h - 1);
        assert(edgeIndex < w - 2);
        if (flip) x = w - edgeIndex - 2;
        else x = edgeIndex + 1;
    } else {
        // vertical dedge
        x = (dedgePos == 1 ? w - 1 : 0);
        assert(edgeIndex < h - 2);
        if (flip) y = h - edgeIndex - 2;
        else y = edgeIndex + 1;
    }
    uint32_t faceIndex = stickermap_index_from_point(sm, face, x, y);
    return faceIndex + stickermap_face_start_index(sm, face);
}

static uint32_t _sm_corner_index(const StickerMap * sm,
                                 int face, int x, int y) {
    int xReal = 0, yReal = 0, w, h;
    stickermap_dimensions_of_face(sm, face, &w, &h);
    if (x == 1) xReal = w - 1;
    if (y == 1) yReal = h - 1;
    
    uint32_t faceIndex = stickermap_index_from_point(sm, face, xReal, yReal);
    return faceIndex + stickermap_face_start_index(sm, face);
}

static uint32_t _sm_center_index(const StickerMap * sm,
                                 int face, int index) {
    int x, y, w, h;
    stickermap_dimensions_of_face(sm, face, &w, &h);
    x = index % (w - 2);
    y = index / (w - 2);
    x++;
    y++;
    uint32_t faceIndex = stickermap_index_from_point(sm, face, x, y);
    return faceIndex + stickermap_face_start_index(sm, face);
}
