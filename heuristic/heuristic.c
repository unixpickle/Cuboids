#include "heuristic.h"

typedef struct {
    int coset;
    int angle;
} HDataAddress;

static int _heuristic_find_subproblem(const char * name, HSubproblem * sp);
static void _generate_symmetry_data(Heuristic * heuristic, HSParameters params);
static int _heuristic_lookup(DataList * list, const uint8_t * data);

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
    _generate_symmetry_data(heuristic, params);
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
    heuristic_angles_free(heuristic->angles);
    
    free(heuristic);
}

/************
 * Metadata *
 ************/

int heuristic_data_size(Heuristic * heuristic) {
    int dataLen = heuristic->subproblem.data_size(heuristic->spUserData);
    if (heuristic->angles->numDistinct > 1) {
        dataLen++;
    }
    return dataLen;
}

/********************
 * Indexer features *
 ********************/

void heuristic_add_coset(Heuristic * heuristic, DataList * coset) {
    assert(coset->headerLen > 0);
    assert(coset->dataSize == heuristic_data_size(heuristic));
    if (!heuristic->cosets) {
        heuristic->cosets = (DataList **)malloc(sizeof(void *));
    } else {
        int newSize = sizeof(void *) * (heuristic->cosetCount + 1);
        heuristic->cosets = (DataList **)realloc(heuristic->cosets, newSize);
    }
    heuristic->cosets[heuristic->cosetCount] = coset;
    heuristic->cosetCount++;
}

void heuristic_get_data(Heuristic * heuristic, const Cuboid * cuboid,
                        int angle, uint8_t * dataOut) {
    heuristic->subproblem.get_data(heuristic->spUserData, cuboid, dataOut, angle);
    if (heuristic->angles->numDistinct > 1) {
        int saveAngle = heuristic->angles->saveAngles[angle];
        dataOut[heuristic_data_size(heuristic) - 1] = saveAngle;
    }
}

/***********
 * Lookups *
 ***********/

int heuristic_pruning_value(Heuristic * heuristic, const Cuboid * cuboid, Cuboid * scratchpad) {
    int angleCount = heuristic->subproblem.angle_count(heuristic->spUserData);
    int dataSize = heuristic_data_size(heuristic);
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
            assert(coset->headerLen > 0);
            for (angle = 0; angle < angleCount; angle++) {
                heuristic_get_data(heuristic, scratchpad, angle, heuristicData);
                int thisValue = _heuristic_lookup(coset, heuristicData);
                if (thisValue < angleValues[angle] && thisValue >= 0) {
                    angleValues[angle] = thisValue;
                }
            }
        }
    }
    
    printf("values %d %d %d.\n", angleValues[0], angleValues[1], angleValues[2]);
    
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

/***********
 * Private *
 ***********/

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

static void _generate_symmetry_data(Heuristic * heuristic, HSParameters params) {
    RotationBasis symmetries = params.symmetries;
    heuristic->symmetries = rotation_group_create_basis(symmetries);
    heuristic->angles = heuristic_angles_for_subproblem(heuristic->subproblem,
                                                        heuristic->spUserData);
}

static int _heuristic_lookup(DataList * list, const uint8_t * data) {
    DataListNode * base = data_list_find_base(list, data, 0);
    if (!base) return -1;
    uint8_t * header;
    if (!data_list_base_find(base, data, &header)) {
        return -1;
    }
    return header[0];
}
