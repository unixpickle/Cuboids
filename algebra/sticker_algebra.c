#include "sticker_algebra.h"

int cuboid_is_center_solid(const Cuboid * cuboid, int face) {
    int centerCount = cuboid_count_centers_for_face(cuboid, face);
    int i, expected = 0;
    for (i = 0; i < centerCount; i++) {
        CuboidCenter c = cuboid->centers[cuboid_center_index(cuboid, face, i)];
        if (expected == 0) {
            expected = c.side;
        } else if (c.side != expected) return 0;
    }
    return 1;
}

int cuboid_is_dedge_paired(const Cuboid * cuboid, int dedge) {
    int edgeCount = cuboid_count_edges_for_dedge(cuboid, dedge);
    int i, expected = -1;
    for (i = 0; i < edgeCount; i++) {
        CuboidEdge e = cuboid->edges[cuboid_edge_index(cuboid, dedge, i)];
        if (expected == -1) {
            expected = e.dedgeIndex;
        } else if (e.dedgeIndex != expected) return 0;
    }
    return 1;
}

int stickermap_face_is_solid_color(const StickerMap * map, int face) {
    int w, h, i;
    stickermap_dimensions_of_face(map, face, &w, &h);
    int startIndex = stickermap_face_start_index(map, face);
    int expected = 0;
    for (i = 0; i < w * h; i++) {
        uint8_t color = map->stickers[i + startIndex];
        if (expected == 0) expected = color;
        else if (expected != color) return 0;
    }
    return 1;
}
