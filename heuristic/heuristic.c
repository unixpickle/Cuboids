#include "heuristic.h"

typedef struct {
    int coset;
    int angle;
} HDataAddress;

static int _heuristic_find_subproblem(const char * name, HSubproblem * sp);

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
    heuristic_initialize_symmetries(heuristic);
    return heuristic;
}

void heuristic_free(Heuristic * heuristic) {
    rotation_group_release(heuristic->dataSymmetries);
    rotation_cosets_release(heuristic->dataCosets);
    
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
                        Cuboid * cache, int angle, uint8_t * dataOut) {
    // loop through each
    assert(rotation_group_count(heuristic->dataSymmetries) == 1 || cache);
    int i, dataSize;
    dataSize = heuristic_data_size(heuristic);
    uint8_t * tempData = (uint8_t *)malloc(dataSize);
    bzero(tempData, dataSize);
    bzero(dataOut, dataSize);
    
    for (i = 0; i < rotation_group_count(heuristic->dataSymmetries); i++) {
        const Cuboid * useCuboid = cuboid;
        if (rotation_group_count(heuristic->dataSymmetries) > 1) {
            Cuboid * symmetry = rotation_group_get(heuristic->dataSymmetries, i);
            cuboid_multiply(cache, symmetry, cuboid);
            useCuboid = cache;
        }
        heuristic_get_raw_data(heuristic, useCuboid, angle, tempData);
        if (heuristic_data_is_gt(tempData, dataOut, dataSize)) {
            memcpy(dataOut, tempData, dataSize);
        }
    }
    free(tempData);
}

void heuristic_get_raw_data(Heuristic * heuristic, const Cuboid * cuboid,
                            int angle, uint8_t * dataOut) {
    heuristic->subproblem.get_data(heuristic->spUserData, cuboid, dataOut, angle);

    if (heuristic->angles->numDistinct > 1) {
        int saveAngle = heuristic->angles->saveAngles[angle];
        dataOut[heuristic_data_size(heuristic) - 1] = saveAngle;
    }
}

void heuristic_initialize_symmetries(Heuristic * heuristic) {
    assert(!heuristic->dataCosets);
    assert(!heuristic->dataSymmetries);
    
    RotationBasis symmetries = heuristic->params.symmetries;
    RotationGroup * fullSymmetries = rotation_group_create_basis(symmetries);
    RotationGroup * subSymmetries = NULL;
    
    if (heuristic->subproblem.data_symmetries) {
        RotationBasis dataBasis;
        void * data = heuristic->spUserData;
        
        dataBasis = heuristic->subproblem.data_symmetries(data);
        assert(rotation_basis_is_subset(symmetries, dataBasis));
        
        subSymmetries = rotation_group_create_basis(dataBasis);
    } else {
        RotationBasis zeroBasis = {symmetries.dims, 0, 0, 0};
        subSymmetries = rotation_group_create_basis(zeroBasis);
    }
    
    heuristic->dataCosets = rotation_cosets_create_right(fullSymmetries, subSymmetries);
    heuristic->dataSymmetries = subSymmetries;
    rotation_group_release(fullSymmetries);
    
    if (!heuristic->angles) {
        heuristic->angles = heuristic_angles_for_subproblem(heuristic->subproblem,
                                                            heuristic->spUserData);
    }
}

int heuristic_data_is_gt(const uint8_t * d1, const uint8_t * d2, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if (d1[i] > d2[i]) return 1;
        if (d1[i] < d2[i]) return 0;
    }
    return 0;
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
    
    Cuboid * extraTemp = NULL;
    if (rotation_group_count(heuristic->dataSymmetries) > 1) {
        extraTemp = cuboid_create(cuboid->dimensions);
    }
    
    for (i = 0; i < rotation_cosets_count(heuristic->dataCosets); i++) {
        Cuboid * symmetry = rotation_cosets_get_trigger(heuristic->dataCosets, i);
        cuboid_multiply(scratchpad, symmetry, cuboid);
        for (cosetIdx = 0; cosetIdx < heuristic->cosetCount; cosetIdx++) {
            DataList * coset = heuristic->cosets[cosetIdx];
            assert(coset->dataSize == dataSize);
            assert(coset->headerLen > 0);
            for (angle = 0; angle < angleCount; angle++) {
                heuristic_get_data(heuristic, scratchpad, extraTemp, angle, heuristicData);
                int thisValue = heuristic_coset_pruning_value(coset, heuristicData);
                if (thisValue < angleValues[angle] && thisValue >= 0) {
                    angleValues[angle] = thisValue;
                }
            }
        }
    }
    if (extraTemp) cuboid_free(extraTemp);
    
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

int heuristic_coset_pruning_value(DataList * list, const uint8_t * data) {
    DataListNode * base = data_list_find_base(list, data, 0);
    if (!base) return -1;
    uint8_t * header;
    if (!data_list_base_find(base, data, &header)) {
        return -1;
    }
    return header[0];
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

