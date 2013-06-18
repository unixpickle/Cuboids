/**
 * This is an identity caching system which makes it very
 * quick and easy to check if a cuboid is solved.
 */

#include "notation/cuboid.h"
#include "power.h"

typedef struct {
    int count;
    Cuboid ** cuboids;
} IdCache;

IdCache * id_cache_create(CuboidDimensions dims);
void id_cache_free(IdCache * cache);
int id_cache_contains(IdCache * cache, const Cuboid * test);
