#include "save_alg_list.h"

static int _load_into_alg_list(AlgList * list, FILE * fp);

void save_alg_list(AlgList * list, FILE * fp) {
    uint32_t count = list->entryCount;
    save_uint32(count, fp);
    int i;
    for (i = 0; i < count; i++) {
        AlgListEntry e = list->entries[i];
        save_algorithm(e.algorithm, fp);
        save_cuboid(e.cuboid, fp);
    }
}

AlgList * load_alg_list(FILE * fp) {
    AlgList * list = alg_list_create();
    if (!_load_into_alg_list(list, fp)) {
        alg_list_release(list);
        return NULL;
    }
    return list;
}

static int _load_into_alg_list(AlgList * list, FILE * fp) {
    uint32_t count;
    if (!load_uint32(&count, fp)) return 0;
    
    int i;
    for (i = 0; i < count; i++) {
        AlgListEntry e;
        e.algorithm = load_algorithm(fp);
        if (!e.algorithm) return 0;
        e.cuboid = load_cuboid(fp);
        if (!e.cuboid) {
            algorithm_free(e.algorithm);
            return 0;
        }
        alg_list_add(list, e);
    }
    
    return 1;
}
