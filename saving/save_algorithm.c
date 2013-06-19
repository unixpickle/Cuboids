#include "save_algorithm.h"

static uint8_t _save_algorithm_type(AlgorithmType type);
static void _save_algorithm_container(const Algorithm * algo, FILE * fp);
static void _save_algorithm_wide_turn(const Algorithm * algo, FILE * fp);
static void _save_algorithm_slice(const Algorithm * algo, FILE * fp);
static void _save_algorithm_rotation(const Algorithm * algo, FILE * fp);

static AlgorithmType _load_algorithm_type(uint8_t type);
static int _load_into_algorithm(Algorithm * algo, FILE * fp);
static int _load_algorithm_container(Algorithm * algo, FILE * fp);
static int _load_algorithm_wide_turn(Algorithm * algo, FILE * fp);
static int _load_algorithm_slice(Algorithm * algo, FILE * fp);
static int _load_algorithm_rotation(Algorithm * algo, FILE * fp);

void save_algorithm(const Algorithm * algo, FILE * fp) {
    uint8_t type = _save_algorithm_type(algo->type);
    fwrite(&type, 1, 1, fp);
    uint32_t power = (uint8_t)algo->power;
    uint8_t inverse = (uint8_t)algo->inverseFlag;
    fwrite(&inverse, 1, 1, fp);
    save_uint32(power, fp);
    
    if (algo->type == AlgorithmTypeContainer) {
        _save_algorithm_container(algo, fp);
    } else if (algo->type == AlgorithmTypeWideTurn) {
        _save_algorithm_wide_turn(algo, fp);
    } else if (algo->type == AlgorithmTypeSlice) {
        _save_algorithm_slice(algo, fp);
    } else if (algo->type == AlgorithmTypeRotation) {
        _save_algorithm_rotation(algo, fp);
    }
}

Algorithm * load_algorithm(FILE * fp) {
    // create a generic brand algorithm
    Algorithm * algo = (Algorithm *)malloc(sizeof(Algorithm));
    bzero(algo, sizeof(Algorithm));
    algo->type = AlgorithmTypeSlice; // something with no free repercussions
    
    if (!_load_into_algorithm(algo, fp)) {
        algorithm_free(algo);
        return NULL;
    }
    return algo;
}

/**********
 * Saving *
 **********/

static uint8_t _save_algorithm_type(AlgorithmType type) {
    if (type == AlgorithmTypeContainer) return 1;
    if (type == AlgorithmTypeWideTurn) return 2;
    if (type == AlgorithmTypeSlice) return 3;
    if (type == AlgorithmTypeRotation) return 4;
    return 0;
}

static void _save_algorithm_container(const Algorithm * algo, FILE * fp) {
    int i, count = algorithm_container_count(algo);
    
    // write the count
    save_uint32((uint32_t)count, fp);
    
    // write the sub-algos
    for (i = 0; i < count; i++) {
        Algorithm * algo1 = algorithm_container_get(algo, i);
        save_algorithm(algo1, fp);
    }
}

static void _save_algorithm_wide_turn(const Algorithm * algo, FILE * fp) {
    char letter = algo->contents.wideTurn.face;
    uint8_t numLayers = algo->contents.wideTurn.numLayers;
    fwrite(&letter, 1, 1, fp);
    fwrite(&numLayers, 1, 1, fp);
}

static void _save_algorithm_slice(const Algorithm * algo, FILE * fp) {
    char letter = algo->contents.slice.layer;
    fwrite(&letter, 1, 1, fp);
}

static void _save_algorithm_rotation(const Algorithm * algo, FILE * fp) {
    char letter = algo->contents.rotation.axis;
    fwrite(&letter, 1, 1, fp);
}

/***********
 * Loading *
 ***********/

static AlgorithmType _load_algorithm_type(uint8_t type) {
    assert(type > 0 && type < 5);
    AlgorithmType types[4] = {AlgorithmTypeContainer, AlgorithmTypeWideTurn,
                              AlgorithmTypeSlice, AlgorithmTypeRotation};
    return types[type - 1];
}

static int _load_into_algorithm(Algorithm * algo, FILE * fp) {
    uint8_t type, inverseFlag;
    uint32_t power;
    if (fread(&type, 1, 1, fp) != 1) return 0;
    if (type < 1 || type > 4) return 0;
    if (fread(&inverseFlag, 1, 1, fp) != 1) return 0;
    if (inverseFlag != 0 && inverseFlag != 1) return 0;
    if (!load_uint32(&power, fp)) return 0;
    algo->power = (int)power;
    algo->inverseFlag = inverseFlag;
    algo->type = _load_algorithm_type(type);
    
    if (algo->type == AlgorithmTypeContainer) {
        return _load_algorithm_container(algo, fp);
    } else if (algo->type == AlgorithmTypeWideTurn) {
        return _load_algorithm_wide_turn(algo, fp);
    } else if (algo->type == AlgorithmTypeSlice) {
        return _load_algorithm_slice(algo, fp);
    } else if (algo->type == AlgorithmTypeRotation) {
        return  _load_algorithm_rotation(algo, fp);
    }
    return 0;
}

static int _load_algorithm_container(Algorithm * algo, FILE * fp) {
    uint32_t count;
    if (!load_uint32(&count, fp)) return 0;
    int i;
    for (i = 0; i < count; i++) {
        Algorithm * a = load_algorithm(fp);
        if (!a) return 0;
        algorithm_container_add(algo, a);
    }
    return 1;
}

static int _load_algorithm_wide_turn(Algorithm * algo, FILE * fp) {
    uint8_t numLayers;
    char face;
    if (fread(&face, 1, 1, fp) != 1) return 0;
    if (fread(&numLayers, 1, 1, fp) != 1) return 0;
    algo->contents.wideTurn.numLayers = numLayers;
    algo->contents.wideTurn.face = face;
    return 1;
}

static int _load_algorithm_slice(Algorithm * algo, FILE * fp) {
    char layer;
    if (fread(&layer, 1, 1, fp) != 1) return 0;
    algo->contents.slice.layer = layer;
    return 1;
}

static int _load_algorithm_rotation(Algorithm * algo, FILE * fp) {
    char axis;
    if (fread(&axis, 1, 1, fp) != 1) return 0;
    algo->contents.rotation.axis = axis;
    return 1;
}
