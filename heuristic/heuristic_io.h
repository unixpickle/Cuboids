#include "heuristic.h"
#include "saving/save_data_list.h"

void save_heuristic(Heuristic * heuristic, FILE * fp);
Heuristic * load_heuristic(FILE * fp, CuboidDimensions newDims);
