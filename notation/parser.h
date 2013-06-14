#include "algorithm.h"
#include <ctype.h>

// a token must not include spaces: i.e. 3Rw2, R', M2
Algorithm * algorithm_for_token(const char * token);

// an algorithm string may include tokens and nested parentheses
Algorithm * algorithm_for_string(const char * buffer);
