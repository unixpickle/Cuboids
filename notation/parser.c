#include "parser.h"

static int _token_num_prefix_len(const char * token);
static int _token_num_suffix_len(const char * token);
static int _token_algo_type(const char name);
static void _token_read_numerical_fixes(const char * token,
                                        int prefLen, int suffLen,
                                        int * prefix, int * suffix);
static Algorithm * _token_parse_slice(const char * token,
                                      int prefLen, int suffLen);
static Algorithm * _token_parse_wide_turn(const char * token,
                                          int prefLen, int suffLen);

Algorithm * algorithm_for_token(const char * token) {
    // read numerical prefix, suffix, etc.
    int numPrefix = _token_num_prefix_len(token);
    int numSuffix = _token_num_suffix_len(token);
    int bodyLength = strlen(token) - numPrefix - numSuffix;
    if (bodyLength <= 0) return NULL;
    if (bodyLength > 3) return NULL;
    
    int type = _token_algo_type(token[numPrefix]);
    if (type < 0) return NULL;
    
    if (type == AlgorithmTypeWideTurn) {
        return _token_parse_wide_turn(token, numPrefix, numSuffix);
    } else if (type == AlgorithmTypeSlice) {
        return _token_parse_slice(token, numPrefix, numSuffix);
    }
    return NULL;
}

// an algorithm string may include tokens and nested parentheses
Algorithm * algorithm_for_string(const char * buffer) {
    return NULL;
}

/********************
 * Token processing *
 ********************/

static int _token_num_prefix_len(const char * token) {
    int count = 0, i;
    for (i = 0; i < strlen(token); i++) {
        if (token[i] < '0' || token[i] > '9') {
            break;
        }
        count++;
    }
    return count;
}

static int _token_num_suffix_len(const char * token) {
    int count = 0, i;
    for (i = strlen(token) - 1; i >= 0; i++) {
        if (token[i] < '0' || token[i] > '9') {
            break;
        }
        count++;
    }
    return count;
}

static int _token_algo_type(const char name) {
    const char * slices = "MES";
    const char * faces = "RULDFB";
    int i;
    for (i = 0; i < strlen(slices); i++) {
        if (slices[i] == name) return AlgorithmTypeSlice;
    }
    for (i = 0; i < strlen(faces); i++) {
        if (faces[i] == name) return AlgorithmTypeWideTurn;
    }
    return -1;
}

static void _token_read_numerical_fixes(const char * token,
                                        int prefLen, int suffLen,
                                        int * prefix, int * suffix) {
    int buffLen = (prefLen > suffLen ? prefLen : suffLen) + 1;
    char * buffer = (char *)malloc(buffLen);
    if (prefLen > 0) {
        bzero(buffer, buffLen);
        memcpy(buffer, token, prefLen);
        *prefix = atoi(buffer);
    }
    if (suffLen > 0) {
        bzero(buffer, buffLen);
        memcpy(buffer, &token[strlen(token) - suffLen], suffLen);
        *suffix = atoi(buffer);
    }
    free(buffer);
}

static Algorithm * _token_parse_slice(const char * token,
                                      int prefLen, int suffLen) {
    int bodyLength = strlen(token) - prefLen - suffLen;
    if (prefLen > 0) return NULL;
    if (bodyLength > 2) return NULL;
    
    // for now, no prefix is allowed for an old-school slice turn
    
    Algorithm * algo = algorithm_new_slice(token[prefLen]);
    if (bodyLength == 2) {
        char nextChar = token[prefLen + 1];
        if (nextChar == '\'') {
            algo->inverseFlag = 1;
        } else {
            algorithm_free(algo);
            return NULL;
        }
    }
    
    if (suffLen > 0) {
        int prefVal, suffVal;
        _token_read_numerical_fixes(token, prefLen, suffLen,
                                    &prefVal, &suffVal);
        algo->power = suffVal;
    }
    
    return algo;
}

static Algorithm * _token_parse_wide_turn(const char * token,
                                          int prefLen, int suffLen) {
    // a wide token must be Xw, #Xw, #Xw'(#), #Xw#, X#, X'(#)
    int bodyLength = strlen(token) - prefLen - suffLen;
    if (bodyLength > 3) return NULL;
    
    int isWideSpecified = 0;
    if (bodyLength > 1) {
        if (token[prefLen + 1] == 'w') isWideSpecified = 1;
    }
    if (!isWideSpecified && prefLen > 0) return NULL;
    
    int prefVal = 1, suffVal = 1;
    _token_read_numerical_fixes(token, prefLen, suffLen, &prefVal, &suffVal);
    
    if (isWideSpecified && prefLen == 0) prefVal = 2;
    
    Algorithm * algo = algorithm_new_wide_turn(token[prefLen], prefVal);
    algo->power = suffVal;
    
    if (bodyLength == 3) {
        if (memcmp(&token[prefLen + 1], "w'", 2) != 0) {
            algorithm_free(algo);
            return NULL;
        }
        algo->inverseFlag = 1;
    } else if (bodyLength == 2 && !isWideSpecified) {
        if (token[prefLen + 1] == '\'') {
            algo->inverseFlag = 1;
        } else {
            algorithm_free(algo);
            return NULL;
        }
    }
    
    return algo;
}
