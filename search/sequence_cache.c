#include "sequence_cache.h"

SequenceCache * sequence_cache_create(Cuboid * baseCuboid, int userCache) {
    SequenceCache * cache = (SequenceCache *)malloc(sizeof(SequenceCache));
    bzero(cache, sizeof(SequenceCache));
    if (userCache) {
        cache->userCache = cuboid_create(baseCuboid->dimensions);
    }
    cache->baseCuboid = baseCuboid;
    return cache;
}

const Cuboid * sequence_cache_make_cuboid(SequenceCache * cache, AlgList * list,
                                    const int * sequence, int len) {
    if (len == 0) return cache->baseCuboid;
    assert(len - cache->lastLength < 2);
    
    int allocLen = cache->cuboidsAlloc;
    if (len > allocLen && allocLen > 0) {
        int newSize = sizeof(Cuboid *) * len;
        cache->cuboids = (Cuboid **)realloc(cache->cuboids, newSize);
        cache->cuboidsAlloc = len;
    } else if (allocLen == 0) {
        int size = sizeof(Cuboid *) * len;
        cache->cuboids = (Cuboid **)malloc(size);
        cache->cuboidsAlloc = len;
    }
    
    int i, start;
    if (len <= cache->lastLength) {
        start = len - 1;
    } else {
        start = cache->lastLength;
    }
    for (i = start; i <= len - 1; i++) {
        Cuboid * opCuboid = list->entries[sequence[i]].cuboid;
        
        // the cuboid at this position might not be allocated already...
        Cuboid * spotCuboid = NULL;
        if (i < allocLen) spotCuboid = cache->cuboids[i];
        else {
            spotCuboid = cuboid_create(cache->baseCuboid->dimensions);
            cache->cuboids[i] = spotCuboid;
        }
        
        if (i == 0) {
            cuboid_multiply(spotCuboid, opCuboid, cache->baseCuboid);
        } else {
            cuboid_multiply(spotCuboid, opCuboid, cache->cuboids[i - 1]);
        }
    }
        
    cache->lastLength = len;
    return cache->cuboids[len - 1];
}

void sequence_cache_clear(SequenceCache * cache) {
    int i;
    for (i = 0; i < cache->cuboidsAlloc; i++) {
        cuboid_free(cache->cuboids[i]);
    }
    free(cache->cuboids);
    cache->lastLength = 0;
    cache->cuboidsAlloc = 0;
}

void sequence_cache_free(SequenceCache * cache) {
    int i;
    for (i = 0; i < cache->cuboidsAlloc; i++) {
        cuboid_free(cache->cuboids[i]);
    }
    if (cache->userCache) {
        cuboid_free(cache->userCache);
    }
    if (cache->cuboids) free(cache->cuboids);
    free(cache);
}
