#include "idcache.h"

static void _id_cache_add(IdCache * cache, Cuboid * c);
static Cuboid * _id_rotation(CuboidDimensions dims, CuboidMovesAxis axis, int * isDouble);
static int _cuboid_light_comparison(const Cuboid * c1, const Cuboid * c2);

IdCache * id_cache_create(CuboidDimensions dims) {    
    IdCache * cache = (IdCache *)malloc(sizeof(IdCache));
    bzero(cache, sizeof(IdCache));
    
    int i, j, k, doubles[3];
    Cuboid * cuboids[3];
    for (i = 0; i < 3; i++) {
        cuboids[i] = _id_rotation(dims, i, &doubles[i]);
    }
    
    // loop through all permutations of our rotations and only add
    // the first cuboid we find of a given configuration...
    for (i = 0; i < (doubles[0] ? 2 : 4); i++) {
        Cuboid * xRot = cuboid_power(cuboids[0], i);
        for (j = 0; j < (doubles[1] ? 2 : 4); j++) {
            Cuboid * yRot = cuboid_power(cuboids[1], j);
            for (k = 0; k < (doubles[2] ? 2 : 4); k++) {                
                Cuboid * zRot = cuboid_power(cuboids[2], k);
                cuboid_multiply_to(yRot, zRot);
                cuboid_multiply_to(xRot, zRot);
                
                if (!id_cache_contains(cache, zRot)) {
                    _id_cache_add(cache, zRot);
                } else {
                    cuboid_free(zRot);
                }
            }
            cuboid_free(yRot);
        }
        cuboid_free(xRot);
    }
    
    return cache;
}

void id_cache_free(IdCache * cache) {
    int i;
    for (i = 0; i < cache->count; i++) {
        cuboid_free(cache->cuboids[i]);
    }
    if (cache->cuboids) free(cache->cuboids);
    free(cache);
}

int id_cache_contains(IdCache * cache, const Cuboid * test) {
    // run a straight forward comparison on each cache :'(
    int i;
    for (i = 0; i < cache->count; i++) {
        if (_cuboid_light_comparison(cache->cuboids[i], test) == 0) {
            return 1;
        }
    }
    return 0;
}

/***********
 * Private *
 ***********/

static void _id_cache_add(IdCache * cache, Cuboid * c) {
    if (cache->count == 0) {
        cache->cuboids = (Cuboid **)malloc(sizeof(Cuboid *));
    } else {
        int newSize = sizeof(Cuboid *) * (cache->count + 1);
        cache->cuboids = (Cuboid **)realloc(cache->cuboids, newSize);
    }
    cache->cuboids[cache->count] = c;
    cache->count++;
}

static Cuboid * _id_rotation(CuboidDimensions dims, CuboidMovesAxis axis, int * isDouble) {
    // who uses one letter variable names now, noobs?
    assert(axis >= 0 && axis < 3);
    const char * axes = "xyz";
    Algorithm * a = algorithm_new_rotation(axes[axis]);
    if (!cuboid_validate_quarter_turn(dims, axis)) {
        isDouble[0] = 1;
        a->power = 2;
    } else {
        isDouble[0] = 0;
    }
    Cuboid * c = algorithm_to_cuboid(a, dims);
    algorithm_free(a);
    return c;
}

static int _cuboid_light_comparison(const Cuboid * c1, const Cuboid * c2) {
    int i;
    int edgeCount = cuboid_count_edges(c1);
    int centerCount = cuboid_count_centers(c1);
    for (i = 0; i < edgeCount; i++) {
        CuboidEdge e1 = c1->edges[i];
        CuboidEdge e2 = c2->edges[i];
        if (e1.dedgeIndex < e2.dedgeIndex) return -1;
        if (e1.dedgeIndex > e2.dedgeIndex) return 1;
        if (e1.symmetry < e2.symmetry) return -1;
        if (e1.symmetry > e2.symmetry) return 1;
    }
    for (i = 0; i < centerCount; i++) {
        CuboidCenter ce1 = c1->centers[i];
        CuboidCenter ce2 = c2->centers[i];
        if (ce1.side < ce2.side) return -1;
        if (ce1.side > ce2.side) return 1;
    }
    for (i = 0; i < 8; i++) {
        CuboidCorner co1 = c1->corners[i];
        CuboidCorner co2 = c2->corners[i];
        if (co1.index < co2.index) return -1;
        if (co1.index > co2.index) return 1;
        if (co1.symmetry < co2.symmetry) return -1;
        if (co1.symmetry > co2.symmetry) return 1;
    }
    return 0;
}
