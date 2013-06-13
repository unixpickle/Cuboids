#include "stickermap.h"
#include "mapconversion_table.h"

typedef struct {
    uint8_t x, y, z;
} Triple;

void convert_sm_to_cb(Cuboid * cuboid, const StickerMap * map);
void convert_cb_to_sm(StickerMap * map, const Cuboid * cuboid);

Triple converter_sm_edge_stickers(const StickerMap * sm,
                                int dedge, int edge);
Triple converter_sm_corner_stickers(const StickerMap * sm,
                                  int corner);

int converter_dedge_for_triple(Triple t, int * symmetry);
int converter_corner_for_triple(Triple t, int * symmetry);
