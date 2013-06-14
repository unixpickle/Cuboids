#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    AlgorithmTypeSlice,
    AlgorithmTypeWideTurn,
    AlgorithmTypeContainer
} AlgorithmType;

// this is my attempt at object-oriented C code.

typedef struct {
    char layer;
} AlgoSliceTurn;

typedef struct {
    char face;
    int numLayers;
} AlgoWideTurn;

typedef struct {
    int childrenCount;
    void ** children;
} AlgoContainer;

typedef struct {
    AlgorithmType type;
    union {
        AlgoSliceTurn slice;
        AlgoWideTurn wideTurn;
        AlgoContainer container;
    } contents;
    uint8_t inverseFlag;
    uint8_t power;
} Algorithm;

Algorithm * algorithm_new_slice(char layer);
Algorithm * algorithm_new_wide_turn(char face, int numLayers);
Algorithm * algorithm_new_container();

void algorithm_container_add(Algorithm * container, Algorithm * a);
void algorithm_container_remove(Algorithm * container, int index);
Algorithm * algorithm_container_get(Algorithm * container, int index);
int algorithm_container_count(Algorithm * container);

void algorithm_free(Algorithm * algo);
