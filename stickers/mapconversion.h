#include "stickermap.h"
#include "mapconversion_table.h"

typedef struct {
    uint8_t x, y, z;
} Triple;

int convert_sm_to_cb(Cuboid * cuboid, const StickerMap * map);
void convert_cb_to_sm(StickerMap * map, const Cuboid * cuboid);

Triple stickermap_get_edge_stickers(const StickerMap * sm,
                                    int dedge, int edge);
Triple stickermap_get_corner_stickers(const StickerMap * sm,
                                      int corner);
void stickermap_set_edge_stickers(StickerMap * sm, int dedge,
                                  int edge, Triple t);
void stickermap_set_corner_stickers(StickerMap * sm,
                                    int corner, Triple t);

int conversion_dedge_for_triple(Triple t, int * symmetry);
int conversion_corner_for_triple(Triple t, int * symmetry);
Triple conversion_triple_for_edge(CuboidEdge edge);
Triple conversion_triple_for_corner(CuboidCorner corner);
