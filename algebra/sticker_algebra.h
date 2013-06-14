#include "stickers/stickermap.h"
#include "representation/cuboid_base.h"

int cuboid_is_center_solid(const Cuboid * cuboid, int face);
int cuboid_is_dedge_paired(const Cuboid * cuboid, int dedge);
int stickermap_face_is_solid_color(const StickerMap * map, int face);
