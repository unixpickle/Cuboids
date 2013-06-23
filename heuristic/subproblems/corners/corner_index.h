#include "heuristic/subproblem_type.h"
#include "arguments/arguments.h"

CLArgumentList * corner_index_default_arguments();
int corner_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData);
int corner_index_load(HSParameters params, FILE * fp, void ** userData);
void corner_index_save(void * userData, FILE * fp);
int corner_index_supports_dimensions(void * userData, CuboidDimensions dims);
int corner_index_data_size(void * userData);
int corner_index_angle_count(void * userData);
void corner_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle);
void corner_index_completed(void * userData);
