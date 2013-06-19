#include "notation/algorithm.h"
#include "save_tools.h"
#include <stdio.h>

void save_algorithm(const Algorithm * algo, FILE * fp);
Algorithm * load_algorithm(FILE * fp);
