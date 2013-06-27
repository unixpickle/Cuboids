#include "heuristic/subproblem_type.h"
#include "arguments/arguments.h"
#include "saving/save_cuboid.h"
#include "saving/save_tools.h"

CLArgumentList * center_index_default_arguments();
int center_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData);
int center_index_load(HSParameters params, FILE * fp, void ** userData);
void center_index_save(void * userData, FILE * fp);
int center_index_supports_dimensions(void * userData, CuboidDimensions dims);
int center_index_data_size(void * userData);
int center_index_angle_count(void * userData);
int center_index_angles_are_equivalent(void * userData, int a1, int a2);
void center_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle);
void center_index_completed(void * userData);
