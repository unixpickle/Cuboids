#include "search/cuboid.h"
#include "save_alg_list.h"
#include <stdio.h>

void save_cuboid_search(CSSearchState * state, FILE * fp);
void save_base_search(BSSearchState * state, FILE * fp);

CSSearchState * load_cuboid_search(FILE * fp);
BSSearchState * load_base_search(FILE * fp);
