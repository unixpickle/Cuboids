#ifndef __STICKERMAP_H__
#define __STICKERMAP_H__

#include <stdint.h>
#include "representation/cuboid_base.h"

/*
 *
 * StickerMaps are useful for representing a cuboid as an array
 * of stickers rather than as a collection of pieces.
 *
 * Conversion between a StickerMap and a Cuboid can be found in
 * mapconversion.h.
 *
*/

typedef struct {
    uint8_t * stickers;
    CuboidDimensions dimensions;
} StickerMap;

StickerMap * stickermap_create(CuboidDimensions dims);
void stickermap_free(StickerMap * map);

// stickers

uint16_t stickermap_count_stickers_for_face(const StickerMap * map, int face);
uint32_t stickermap_count_stickers(const StickerMap * map);
uint32_t stickermap_face_start_index(const StickerMap * map, int face);

// face geometry

void stickermap_dimensions_of_face(const StickerMap * map, int face, int * w, int * h);
void stickermap_index_to_point(const StickerMap * map, int face,
                               int faceIndex, int * x, int * y);
int stickermap_index_from_point(const StickerMap * map,
                                int face, int x, int y);

#endif
                                