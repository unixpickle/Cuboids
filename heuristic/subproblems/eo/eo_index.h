#include "heuristic/subproblem_type.h"
#include "arguments/arguments.h"
#include "saving/save_cuboid.h"
#include "saving/save_tools.h"
#include "pieces/eo.h"

CLArgumentList * eo_index_default_arguments();
int eo_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData);
int eo_index_load(HSParameters params, FILE * fp, void ** userData);
void eo_index_save(void * userData, FILE * fp);
int eo_index_supports_dimensions(void * userData, CuboidDimensions dims);
int eo_index_data_size(void * userData);
int eo_index_angle_count(void * userData);
int eo_index_angles_are_equivalent(void * userData, int a1, int a2);
void eo_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle);
void eo_index_completed(void * userData);
RotationBasis eo_index_data_symmetries(void * userData);
