/**
 * The sequence cache makes the cuboid searcher more efficient by
 * keeping up to date with the sequences that the base searcher
 * is looking at and continually recomputing Cuboid* objects for
 * these sequences.
 */

#include <assert.h>
#include "notation/alg_list.h"

typedef struct {
    const Cuboid * baseCuboid;
    
    Cuboid * userCache;
    Cuboid ** cuboids;
    int cuboidsAlloc;
    int lastLength;
} SequenceCache;

SequenceCache * sequence_cache_create(Cuboid * baseCuboid, int userCache);
const Cuboid * sequence_cache_make_cuboid(SequenceCache * cache, AlgList * list,
                                    const int * sequence, int len);
void sequence_cache_clear(SequenceCache * cache);
void sequence_cache_free(SequenceCache * cache);
