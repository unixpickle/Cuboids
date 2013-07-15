#include "heuristic_buffer.h"

HeuristicBuffer * heuristic_buffer_create(Heuristic * heuristic) {
    HeuristicBuffer * buffer = (HeuristicBuffer *)malloc(sizeof(HeuristicBuffer));
    buffer->angleCount = heuristic->angles->numAngles;
    buffer->cosetCount = rotation_cosets_count(heuristic->dataCosets);
    buffer->dataSize = heuristic_data_size(heuristic);
    
    // allocate everything in the buffer
    buffer->data = (uint8_t ***)malloc(sizeof(uint8_t **) * buffer->angleCount);
    int i, j;
    for (i = 0; i < buffer->angleCount; i++) {
        buffer->data[i] = (uint8_t **)malloc(sizeof(uint8_t *) * buffer->cosetCount);
        for (j = 0; j < buffer->cosetCount; j++) {
            buffer->data[i][j] = (uint8_t *)malloc(buffer->dataSize);
            bzero(buffer->data[i][j], buffer->dataSize);
        }
    }
    
    buffer->heuristic = heuristic;
    
    return buffer;
}

void heuristic_buffer_free(HeuristicBuffer * buffer) {
    int i, j;
    for (i = 0; i < buffer->angleCount; i++) {
        for (j = 0; j < buffer->cosetCount; j++) {
            free(buffer->data[i][j]);
        }
        free(buffer->data[i]);
    }
    free(buffer->data);
    free(buffer);
}

void heuristic_buffer_add(HeuristicBuffer * buffer, const Cuboid * cb, int coset) {
    int angle = 0;
    uint8_t * temp = (uint8_t *)malloc(buffer->dataSize);
    for (angle = 0; angle < buffer->angleCount; angle++) {
        heuristic_get_raw_data(buffer->heuristic, cb, angle, temp);
        // check if it's better than our current data
        uint8_t * dataDest = buffer->data[angle][coset];
        if (heuristic_data_is_gt(temp, dataDest, buffer->dataSize)) {
            memcpy(dataDest, temp, buffer->dataSize);
        }
    }
    free(temp);
}

int heuristic_buffer_pruning_value(HeuristicBuffer * buffer) {
    int angle, currentValue = 0;
    for (angle = 0; angle < buffer->angleCount; angle++) {
        int i, j, value = buffer->heuristic->params.maxDepth + 1;
        for (i = 0; i < buffer->cosetCount; i++) {
            uint8_t * data = buffer->data[angle][i];
            for (j = 0; j < buffer->heuristic->cosetCount; j++) {
                DataList * coset = buffer->heuristic->cosets[j];
                int theValue = heuristic_coset_pruning_value(coset, data);
                if (theValue >= 0 && theValue < value) {
                    value = theValue;
                }
            }
        }
        if (value > currentValue) {
            currentValue = value;
        }
    }
    return currentValue;
}