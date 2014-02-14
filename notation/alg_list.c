#include "alg_list.h"

AlgList * alg_list_create() {
    AlgList * list = (AlgList *)malloc(sizeof(AlgList));
    bzero(list, sizeof(AlgList));
    list->retainCount = 1;
    return list;
}

void alg_list_add(AlgList * list, AlgListEntry entry) {
    if (!list->entries) {
        list->entries = (AlgListEntry *)malloc(sizeof(AlgListEntry));
    } else {
        int newSize = sizeof(AlgListEntry) * (list->entryCount + 1);
        list->entries = (AlgListEntry *)realloc(list->entries, newSize);
    }
    list->entries[list->entryCount] = entry;
    list->entryCount++;
}

void alg_list_release(AlgList * list) {
    list->retainCount--;
    if (list->retainCount > 0) return;
    int i;
    for (i = 0; i < list->entryCount; i++) {
        algorithm_free(list->entries[i].algorithm);
        cuboid_free(list->entries[i].cuboid);
    }
    if (list->entries) free(list->entries);
    free(list);
}

void alg_list_retain(AlgList * list) {
    list->retainCount++;
}

AlgList * alg_list_parse(const char * buffer, CuboidDimensions dims) {
    char * tmpBuffer = (char *)malloc(strlen(buffer) + 1);
    bzero(tmpBuffer, strlen(buffer) + 1);
    
    AlgList * list = alg_list_create();
    
    // add each element separated by commas
    
    int i;
    for (i = 0; i < (int)strlen(buffer); i++) {
        if (buffer[i] == ',' && strlen(tmpBuffer) > 0) {
            Algorithm * algo = algorithm_for_string(tmpBuffer);
            if (!algo) {
                alg_list_release(list);
                free(tmpBuffer);
                return NULL;
            }
            Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
            if (!cuboid) {
                algorithm_free(algo);
                alg_list_release(list);
                free(tmpBuffer);
                return NULL;
            }
            AlgListEntry e;
            e.algorithm = algo;
            e.cuboid = cuboid;
            alg_list_add(list, e);
            tmpBuffer[0] = 0;
        } else {
            tmpBuffer[strlen(tmpBuffer) + 1] = 0;
            tmpBuffer[strlen(tmpBuffer)] = buffer[i];
        }
    }
    
    // add the last element
    if (strlen(tmpBuffer) > 0) {
        Algorithm * algo = algorithm_for_string(tmpBuffer);
        if (!algo) {
            alg_list_release(list);
            free(tmpBuffer);
            return NULL;
        }
        Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
        if (!cuboid) {
            algorithm_free(algo);
            alg_list_release(list);
            free(tmpBuffer);
            return NULL;
        }
        AlgListEntry e;
        e.algorithm = algo;
        e.cuboid = cuboid;
        alg_list_add(list, e);
    }
    
    free(tmpBuffer);
    
    return list;
}
