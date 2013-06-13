#include "stickermap.h"

StickerMap * stickermap_create(CuboidDimensions dims) {
    int stickerCount = 0;
    stickerCount += 2 * dims.x * dims.y;
    stickerCount += 2 * dims.x * dims.z;
    stickerCount += 2 * dims.y * dims.z;
    StickerMap * map = (StickerMap *)malloc(sizeof(StickerMap));
    map->stickers = malloc(stickerCount);;
    map->dimensions = dims;
    return map;
}

void stickermap_free(StickerMap * map) {
    free(map->stickers);
    free(map);
}

uint16_t stickermap_count_stickers_for_face(const StickerMap * map, int face) {
    if (face == 1 || face == 2) {
        return map->dimensions.x * map->dimensions.y;
    } else if (face == 3 || face == 4) {
        return map->dimensions.z * map->dimensions.x;
    } else {
        return map->dimensions.y * map->dimensions.z;
    }
}

uint32_t stickermap_count_stickers(const StickerMap * map) {
    CuboidDimensions dims = map->dimensions;
    uint32_t stickerCount = 0;
    stickerCount += 2 * dims.x * dims.y;
    stickerCount += 2 * dims.x * dims.z;
    stickerCount += 2 * dims.y * dims.z;
    return stickerCount;
}

uint32_t stickermap_face_start_index(const StickerMap * map, int face) {
    int countBefore = 0, i;
    for (i = 1; i < face; i++) {
        countBefore += stickermap_count_stickers_for_face(map, i);
    }
    return countBefore;
}

/*****************
 * Face geometry *
 *****************/

void stickermap_dimensions_of_face(const StickerMap * map, int face, int * w, int * h) {
    if (face == 1 || face == 2) {
        *w = map->dimensions.x;
        *h = map->dimensions.y;
    } else if (face == 3 || face == 4) {
        *w = map->dimensions.x;
        *h = map->dimensions.z;
    } else {
        *w = map->dimensions.z;
        *h = map->dimensions.y;
    }
}

void stickermap_index_to_point(const StickerMap * map, int face, int faceIndex, int * x, int * y) {
    int w, h;
    stickermap_dimensions_of_face(map, face, &w, &h);
    assert(faceIndex < w * h && faceIndex >= 0);
    *x = faceIndex % w;
    *y = faceIndex / h;
}

int stickermap_index_from_point(const StickerMap * map, int face, int x, int y) {
    int w, h;
    stickermap_dimensions_of_face(map, face, &w, &h);
    assert(x < w && y < h && x >= 0 && y >= 0);
    return x + (y * w);
}
