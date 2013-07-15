#include "heuristic_list.h"

static void _generate_coset_map(Heuristic * heuristic, HeuristicCosetMap * map, 
                                RotationGroup * allSymmetries, Cuboid * cache);
static RotationBasis _rotation_basis_container(RotationBasis b1, RotationBasis b2);

HeuristicList * heuristic_list_new() {
    HeuristicList * list = (HeuristicList *)malloc(sizeof(HeuristicList));
    bzero(list, sizeof(HeuristicList));
    return list;
}

void heuristic_list_free(HeuristicList * list) {
    if (list->dataSymmetries) {
        rotation_group_release(list->dataSymmetries);
    }
    if (list->cosetMaps) {
        int i;
        for (i = 0; i < list->count; i++) {
            free(list->cosetMaps[i].cosets);
        }
        free(list->cosetMaps);
    }
    free(list);
}

void heuristic_list_add(HeuristicList * list, Heuristic * h, const char * file) {
    if (!list->heuristics) {
        list->heuristics = (Heuristic **)malloc(sizeof(void *));
    } else {
        int newSize = (list->count + 1) * sizeof(void *);
        list->heuristics = (Heuristic **)realloc(list->heuristics, newSize);
    }
    if (!list->fileNames) {
        list->fileNames = (char **)malloc(sizeof(void *));
    } else {
        int newSize = (list->count + 1) * sizeof(void *);
        list->fileNames = (char **)realloc(list->fileNames, newSize);
    }
    list->heuristics[list->count] = h;
    list->fileNames[list->count] = (char *)malloc(strlen(file) + 1);
    strcpy(list->fileNames[list->count], file);
    list->count++;
    
    // expand our list's dataBasis to enclude all of the data symmetries
    RotationBasis dataBasis = h->params.symmetries;
    list->dataBasis = _rotation_basis_container(dataBasis, list->dataBasis);
}

/***********
 * Lookups *
 ***********/

void heuristic_list_prepare(HeuristicList * list, Cuboid * cache) {
    if (list->count == 0) return;
    
    list->dataSymmetries = rotation_group_create_basis(list->dataBasis);
    
    // generate coset maps for each heuristic
    int mapsSize = sizeof(HeuristicCosetMap) * list->count;
    list->cosetMaps = (HeuristicCosetMap *)malloc(mapsSize);
    int i;
    for (i = 0; i < list->count; i++) {
        _generate_coset_map(list->heuristics[i], &list->cosetMaps[i],
                            list->dataSymmetries, cache);
    }
}

int heuristic_list_pruning_value(HeuristicList * list, const Cuboid * cuboid,
                                 Cuboid * cache) {
    if (list->count == 0) return 0;
    
    int i, j, buffersSize = sizeof(HeuristicBuffer *) * list->count;
    HeuristicBuffer ** buffers = (HeuristicBuffer **)malloc(buffersSize);
    for (i = 0; i < list->count; i++) {
        buffers[i] = heuristic_buffer_create(list->heuristics[i]);
    }
    
    // loop through all the symmetries and push it to each buffer
    for (i = 0; i < rotation_group_count(list->dataSymmetries); i++) {
        const Cuboid * rotation = rotation_group_get(list->dataSymmetries, i);
        cuboid_multiply(cache, rotation, cuboid);
        // loop through each buffer and handle this cuboid if
        // the heuristic is interested
        for (j = 0; j < list->count; j++) {
            int coset = list->cosetMaps[j].cosets[i];
            if (coset < 0) continue;
            HeuristicBuffer * buffer = buffers[j];
            heuristic_buffer_add(buffer, cache, coset);
        }
    }
    
    int pruningValue = 0;
    for (i = 0; i < list->count; i++) {
        HeuristicBuffer * buffer = buffers[i];
        int value = heuristic_buffer_pruning_value(buffer);
        if (value > pruningValue) {
            pruningValue = value;
        }
    }
    
    for (i = 0; i < list->count; i++) {
        heuristic_buffer_free(buffers[i]);
    }
    free(buffers);
    
    return pruningValue;
}

int heuristic_list_exceeds(HeuristicList * list, const Cuboid * cuboid,
                           Cuboid * cache, int maxValue) {
    if (list->count == 0) return 0;
    
    // allocate resources
    int i, j, buffersSize = sizeof(HeuristicBuffer *) * list->count;
    HeuristicBuffer ** buffers = (HeuristicBuffer **)malloc(buffersSize);
    for (i = 0; i < list->count; i++) {
        buffers[i] = heuristic_buffer_create(list->heuristics[i]);
    }
    
    int rotationsSize = sizeof(Cuboid *) * rotation_group_count(list->dataSymmetries);
    Cuboid ** rotations = (Cuboid **)malloc(rotationsSize);
    
    // apply all rotations to the cuboid
    for (i = 0; i < rotation_group_count(list->dataSymmetries); i++) {
        const Cuboid * rotation = rotation_group_get(list->dataSymmetries, i);
        cuboid_multiply(cache, rotation, cuboid);
        rotations[i] = cuboid_copy(cache);
    }
    
    int exceeds = 0;
    // test all heuristics
    for (i = 0; i < list->count; i++) {
        HeuristicBuffer * buffer = buffers[i];
        for (j = 0; j < rotation_group_count(list->dataSymmetries); j++) {
            int coset = list->cosetMaps[i].cosets[j];
            heuristic_buffer_add(buffer, rotations[j], coset);
        }
        int value = heuristic_buffer_pruning_value(buffer);
        if (value > maxValue) {
            exceeds = 1;
            break;
        }
    }
    
    // free resources
    for (i = 0; i < list->count; i++) {
        heuristic_buffer_free(buffers[i]);
    }
    free(buffers);
    for (i = 0; i < rotation_group_count(list->dataSymmetries); i++) {
        cuboid_free(rotations[i]);
    }
    free(rotations);
    
    return exceeds;
}

/***********
 * Private *
 ***********/

static void _generate_coset_map(Heuristic * heuristic, HeuristicCosetMap * map,
                                RotationGroup * allSymmetries, Cuboid * cache) {
    int coset, i, j, k;
    
    // clear all values
    map->cosets = (int8_t *)malloc(sizeof(int8_t) * rotation_group_count(allSymmetries));
    for (i = 0; i < rotation_group_count(allSymmetries); i++) {
        map->cosets[i] = -1;
    }
    
    // generate all of the data rotations
    for (coset = 0; coset < rotation_cosets_count(heuristic->dataCosets); coset++) {
        const Cuboid * trigger = rotation_cosets_get_trigger(heuristic->dataCosets, coset);
        for (j = 0; j < rotation_group_count(heuristic->dataSymmetries); j++) {
            const Cuboid * symmetry = rotation_group_get(heuristic->dataSymmetries, j);
            cuboid_multiply(cache, symmetry, trigger);
            
            // figure out where it is in the list
            for (k = 0; k < rotation_group_count(allSymmetries); k++) {
                Cuboid * testSymmetry = rotation_group_get(allSymmetries, k);
                if (cuboid_light_comparison(testSymmetry, cache) == 0) {
                    map->cosets[k] = coset;
                    break;
                }
            }
        }
    }
}

static RotationBasis _rotation_basis_container(RotationBasis b1, RotationBasis b2) {
    RotationBasis newBasis = b1;
    
    if (b2.xPower == 1) newBasis.xPower = 1;
    else if (b2.xPower == 2 && newBasis.xPower != 1) newBasis.xPower = 2;
    
    if (b2.yPower == 1) newBasis.yPower = 1;
    else if (b2.yPower == 2 && newBasis.yPower != 1) newBasis.yPower = 2;
    
    if (b2.zPower == 1) newBasis.zPower = 1;
    else if (b2.zPower == 2 && newBasis.zPower != 1) newBasis.zPower = 2;
    
    return newBasis;
}
