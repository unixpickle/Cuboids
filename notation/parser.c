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
static Algorithm * _token_parse_rotation(const char * token,
                                         int prefLen, int suffLen);

static Algorithm * _algo_read_nested_tokens(const char * str, int * lenOut);
static Algorithm * _algo_read_next_token(const char * str, int * lenOut);
static int _algo_next_token_length(const char * str);
static int _parse_number_of_length(const char * str, int len);

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
    } else if (type == AlgorithmTypeRotation) {
        return _token_parse_rotation(token, numPrefix, numSuffix);
    }
    return NULL;
}

// an algorithm string may include tokens and nested parentheses
Algorithm * algorithm_for_string(const char * buffer) {
    int usedLen;
    Algorithm * a = _algo_read_nested_tokens(buffer, &usedLen);
    
    if (usedLen != (int)strlen(buffer)) {
        if (a) algorithm_free(a);
        return NULL;
    }
    return a;
}

/********************
 * Token processing *
 ********************/

static int _token_num_prefix_len(const char * token) {
    int count = 0, i;
    for (i = 0; i < (int)strlen(token); i++) {
        if (token[i] < '0' || token[i] > '9') {
            break;
        }
        count++;
    }
    return count;
}

static int _token_num_suffix_len(const char * token) {
    int count = 0, i;
    for (i = strlen(token) - 1; i >= 0; i--) {
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
    const char * rotations = "xyz";
    int i;
    for (i = 0; i < (int)strlen(slices); i++) {
        if (slices[i] == name) return AlgorithmTypeSlice;
    }
    for (i = 0; i < (int)strlen(faces); i++) {
        if (faces[i] == name) return AlgorithmTypeWideTurn;
    }
    for (i = 0; i < (int)strlen(rotations); i++) {
        if (rotations[i] == name) return AlgorithmTypeRotation;
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

static Algorithm * _token_parse_rotation(const char * token,
                                         int prefLen, int suffLen) {
    if (prefLen > 0) return NULL;
    int bodyLen = strlen(token) - suffLen;
    if (bodyLen > 2) return NULL;
    
    if (bodyLen == 2) {
        if (token[1] != '\'') return NULL;
    }
    Algorithm * a = algorithm_new_rotation(token[0]);
    a->inverseFlag = (bodyLen == 2);
    
    if (suffLen > 0) {
        a->power = _parse_number_of_length(&token[strlen(token) - suffLen], suffLen);
    }
    
    return a;
}

/*********************
 * Algorithm strings *
 *********************/

static Algorithm * _algo_read_nested_tokens(const char * str, int * lenOut) {
    Algorithm * container = algorithm_new_container();
    *lenOut = (int)strlen(str);
    
    int i;
    for (i = 0; i < (int)strlen(str); i++) {
        if (isspace(str[i])) continue;
        // handle recursion characters
        if (str[i] == '(') {
            int len;
            Algorithm * subAlgo = _algo_read_nested_tokens(&str[i + 1], &len);
            if (!subAlgo) {
                algorithm_free(container);
                return NULL;
            }
            algorithm_container_add(container, subAlgo);
            i += len;
            continue;
        } else if (str[i] == ')') {
            // check for a number after the parentheses
            int digitCount = _token_num_prefix_len(&str[i + 1]);
            if (digitCount > 0) {
                *lenOut = i + 1 + digitCount;
                container->power = _parse_number_of_length(&str[i + 1], digitCount);
            } else {
                *lenOut = i + 1;
            }
            break;
        }
        
        // this is (at least, it should be) a normal token
        int lenOut = 0;
        Algorithm * nextToken = _algo_read_next_token(&str[i], &lenOut);
        i += lenOut - 1;
        
        if (!nextToken) {
            algorithm_free(container);
            return NULL;
        } else {
            algorithm_container_add(container, nextToken);
        }
    }
    return container;
}

static Algorithm * _algo_read_next_token(const char * str, int * lenOut) {
    int nextLen = _algo_next_token_length(str);
    char * buffer = (char *)malloc(nextLen + 1);
    buffer[nextLen] = 0;
    memcpy(buffer, str, nextLen);
    *lenOut = nextLen;
    
    Algorithm * a = algorithm_for_token(buffer);
    
    free(buffer);
    return a;
}

static int _algo_next_token_length(const char * str) {
    int i;
    for (i = 0; i < (int)strlen(str); i++) {
        if (isspace(str[i])) return i;
        if (str[i] == ')') return i;
    }
    return strlen(str);
}

static int _parse_number_of_length(const char * str, int len) {
    char * buffer = (char *)malloc(len + 1);
    buffer[len] = 0;
    memcpy(buffer, str, len);
    int number = atoi(buffer);
    free(buffer);
    return number;
}
