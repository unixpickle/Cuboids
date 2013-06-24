#ifndef __HEURISTIC_IO_H__
#define __HEURISTIC_IO_H__

#include "heuristic_list.h"
#include "heuristic.h"
#include "saving/save_data_list.h"

void save_heuristic(Heuristic * heuristic, FILE * fp);
Heuristic * load_heuristic(FILE * fp, CuboidDimensions newDims);
Heuristic * heuristic_from_file(const char * fileName, CuboidDimensions dims);

// saves the file name of each heuristic
void save_heuristic_list(HeuristicList * list, FILE * fp);
HeuristicList * load_heuristic_list(FILE * fp, CuboidDimensions dims);

#endif