#ifndef __HEURISTIC_IO_H__
#define __HEURISTIC_IO_H__

#include "heuristic.h"
#include "saving/save_data_list.h"

void save_heuristic(Heuristic * heuristic, FILE * fp);
Heuristic * load_heuristic(FILE * fp, CuboidDimensions newDims);
Heuristic * heuristic_from_file(const char * fileName, CuboidDimensions dims);

#endif