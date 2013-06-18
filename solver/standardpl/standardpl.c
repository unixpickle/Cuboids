#include "standardpl.h"

CLArgumentList * standardpl_default_arguments() {
    return cl_argument_list_new();
}

int standardpl_run(CLSearchParameters * params, CLArgumentList * list, void ** userData) {
    if (list->count > 0) {
        return 0;
    }
    
    IdCache * cache = id_cache_create(params->dimensions);
    *userData = cache;
    
    return 1;
}

int standardpl_resume(CLSearchParameters * params, FILE * fp, void ** userData) {
    IdCache * cache = id_cache_create(params->dimensions);
    *userData = cache;
    return 1;
}

void standardpl_save(void * data, FILE * fp) {
    // nothing to be done
}

void standardpl_completed(void * data) {
    IdCache * cache = (IdCache *)data;
    id_cache_free(cache);
}

int standardpl_is_goal(void * data, Cuboid * cb, StickerMap * shouldBeNull) {
    assert(shouldBeNull == NULL);
    IdCache * cache = (IdCache *)data;
    return id_cache_contains(cache, cb);
}
