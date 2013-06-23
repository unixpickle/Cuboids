#include "heuristic.h"

static int _heuristic_find_subproblem(const char * name, HSubproblem * sp);
static void _generate_symmetries(Heuristic * heuristic, HSParameters params);
static int _heuristic_lookup(DataList * list, const uint8_t * data, int maxDepth);
static int _heuristic_lookup_or_fail(DataList * list, const uint8_t * data);

Heuristic * heuristic_create(HSParameters params, CLArgumentList * args, const char * spName) {
    HSubproblem subproblem;
    if (!_heuristic_find_subproblem(spName, &subproblem)) {
        return NULL;
    }
    void * userData = NULL;
    if (!subproblem.initialize(params, args, &userData)) {
        return NULL;
    }
    Heuristic * heuristic = (Heuristic *)malloc(sizeof(Heuristic));
    bzero(heuristic, sizeof(Heuristic));
    heuristic->subproblem = subproblem;
    heuristic->spUserData = userData;
    heuristic->params = params;
    _generate_symmetries(heuristic, params);
    return heuristic;
}

void heuristic_free(Heuristic * heuristic) {
    rotation_group_release(heuristic->symmetries);
    
    if (heuristic->cosets) {
        int i;
        for (i = 0; i < heuristic->cosetCount; i++) {
            data_list_free(heuristic->cosets[i]);
        }
        free(heuristic->cosets);
    }
    
    heuristic->subproblem.completed(heuristic->spUserData);
    
    free(heuristic);
}

void heuristic_add_coset(Heuristic * heuristic, DataList * coset) {
    if (!heuristic->cosets) {
        heuristic->cosets = (DataList **)malloc(sizeof(void *));
    } else {
        int newSize = sizeof(void *) * (heuristic->cosetCount + 1);
        heuristic->cosets = (DataList **)realloc(heuristic->cosets, newSize);
    }
    heuristic->cosets[heuristic->cosetCount] = coset;
    heuristic->cosetCount++;
}

int heuristic_pruning_value(Heuristic * heuristic, const Cuboid * cuboid, Cuboid * scratchpad) {
    int angleCount = heuristic->subproblem.angle_count(heuristic->spUserData);
    int dataSize = heuristic->subproblem.data_size(heuristic->spUserData);
    int * angleValues = (int *)malloc(sizeof(int) * angleCount);
    uint8_t * heuristicData = (uint8_t *)malloc(dataSize);
    
    bzero(heuristicData, dataSize);
    
    int i, angle, cosetIdx;
    
    for (i = 0; i < angleCount; i++) {
        angleValues[i] = heuristic->params.maxDepth + 1;
    }
    
    for (i = 0; i < rotation_group_count(heuristic->symmetries); i++) {
        Cuboid * symmetry = rotation_group_get(heuristic->symmetries, i);
        cuboid_multiply(scratchpad, symmetry, cuboid);
        for (cosetIdx = 0; cosetIdx < heuristic->cosetCount; cosetIdx++) {
            DataList * coset = heuristic->cosets[cosetIdx];
            assert(coset->dataSize == dataSize);
            assert(coset->headerLen == 2);
            for (angle = 0; angle < angleCount; angle++) {
                heuristic->subproblem.get_data(heuristic->spUserData, scratchpad,
                                               heuristicData, angle);
                int thisValue = _heuristic_lookup(coset, heuristicData,
                                                  heuristic->params.maxDepth);
                if (thisValue < angleValues[angle]) {
                    angleValues[angle] = thisValue;
                }
            }
        }
    }
    
    int maxValue = 0;
    for (i = 0; i < angleCount; i++) {
        if (angleValues[i] > maxValue) {
            maxValue = angleValues[i];
        }
    }
    
    free(heuristicData);
    free(angleValues);
    return maxValue;
}

int heuristic_coset_value(Heuristic * heuristic, const Cuboid * cuboid,
                          int cosetIdx, int angle) {
    assert(cosetIdx >= 0 && cosetIdx < heuristic->cosetCount);
    DataList * coset = heuristic->cosets[cosetIdx];
    int dataSize = heuristic->subproblem.data_size(heuristic->spUserData);
    uint8_t * heuristicData = (uint8_t *)malloc(dataSize);
    
    heuristic->subproblem.get_data(heuristic->spUserData, cuboid,
                                   heuristicData, angle);

    int value = _heuristic_lookup_or_fail(coset, heuristicData);
    free(heuristicData);
    return value;
}

static int _heuristic_find_subproblem(const char * name, HSubproblem * sp) {
    int spCount = sizeof(HSubproblemTable) / sizeof(HSubproblem);
    int i;
    for (i = 0; i < spCount; i++) {
        HSubproblem subproblem = HSubproblemTable[i];
        if (strcmp(subproblem.name, name) == 0) {
            *sp = subproblem;
            return 1;
        }
    }
    return 0;
}

static void _generate_symmetries(Heuristic * heuristic, HSParameters params) {
    RotationBasis symmetries = params.symmetries;
    heuristic->symmetries = rotation_group_create_basis(symmetries);
}

static int _heuristic_lookup(DataList * list, const uint8_t * data, int maxDepth) {
    int result = _heuristic_lookup_or_fail(list, data);
    if (result < 0) return maxDepth + 1;
    return result;
}

static int _heuristic_lookup_or_fail(DataList * list, const uint8_t * data) {
    DataListNode * base = data_list_find_base(list, data, 0);
    if (!base) return -1;
    uint8_t * header;
    if (!data_list_base_find(base, data, &header)) {
        return -1;
    }
    return header[0] | (header[1] << 8);
}
