#include "algorithm.h"

Algorithm * algorithm_new_slice(char layer) {
    Algorithm * algo = (Algorithm *)malloc(sizeof(Algorithm));
    bzero(algo, sizeof(Algorithm));
    algo->type = AlgorithmTypeSlice;
    algo->contents.slice.layer = layer;
    algo->power = 1;
    return algo;
}

Algorithm * algorithm_new_wide_turn(char face, int numLayers) {
    Algorithm * algo = (Algorithm *)malloc(sizeof(Algorithm));
    bzero(algo, sizeof(Algorithm));
    algo->type = AlgorithmTypeWideTurn;
    algo->contents.wideTurn.face = face;
    algo->contents.wideTurn.numLayers = numLayers;
    algo->power = 1;
    return algo;
}

Algorithm * algorithm_new_rotation(char axis) {
    Algorithm * algo = (Algorithm *)malloc(sizeof(Algorithm));
    bzero(algo, sizeof(Algorithm));
    algo->type = AlgorithmTypeRotation;
    algo->contents.rotation.axis = axis;
    algo->power = 1;
    return algo;
}

Algorithm * algorithm_new_container() {
    Algorithm * algo = (Algorithm *)malloc(sizeof(Algorithm));
    bzero(algo, sizeof(Algorithm));
    algo->type = AlgorithmTypeContainer;
    algo->power = 1;
    return algo;
}

/**************
 * Containers *
 **************/

void algorithm_container_add(Algorithm * container, Algorithm * a) {
    assert(container->type == AlgorithmTypeContainer);
    if (container->contents.container.children) {
        void ** children = container->contents.container.children;
        int count = algorithm_container_count(container);
        int newSize = sizeof(void *) * (count + 1);
        children = realloc(children, newSize);
        count++;
        container->contents.container.children = children;
        container->contents.container.childrenCount = count;
    } else {
        container->contents.container.children = (void **)malloc(sizeof(void *));
        container->contents.container.childrenCount = 1;
    }
    int index = container->contents.container.childrenCount - 1;
    container->contents.container.children[index] = a;
}

void algorithm_container_remove(Algorithm * container, int index) {
    assert(container->type == AlgorithmTypeContainer);
    
    int totalCopied = 0, i;
    int count = algorithm_container_count(container);
    
    assert(index >= 0 && index < count);
    
    for (i = 0; i < count; i++) {
        if (i == index) continue;
        void * newPtr = container->contents.container.children[i];
        container->contents.container.children[totalCopied] = newPtr;
        totalCopied++;
    }
    void ** newChildren = realloc(container->contents.container.children,
                                  sizeof(void *) * totalCopied);
    container->contents.container.childrenCount = totalCopied;
    container->contents.container.children = newChildren;
}

Algorithm * algorithm_container_get(const Algorithm * container, int index) {
    assert(container->type == AlgorithmTypeContainer);
    assert(index >= 0 && index < algorithm_container_count(container));
    return container->contents.container.children[index];
}

int algorithm_container_count(const Algorithm * container) {
    assert(container->type == AlgorithmTypeContainer);
    return container->contents.container.childrenCount;
}

/**********
 * Memory *
 **********/

void algorithm_free(Algorithm * algo) {
    if (algo->type == AlgorithmTypeContainer) {
        int i;
        for (i = 0; i < algo->contents.container.childrenCount; i++) {
            Algorithm * a = (Algorithm *)algo->contents.container.children[i];
            algorithm_free(a);
        }
        if (algo->contents.container.children) {
            free(algo->contents.container.children);
        }
    }
    free(algo);
}
