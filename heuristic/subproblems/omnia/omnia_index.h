/*
 * Btw Omnia means "everything" in Latin
 */

#include "heuristic/subproblem_type.h"
#include "arguments/arguments.h"
#include "saving/save_cuboid.h"

CLArgumentList * omnia_index_default_arguments();
int omnia_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData);
int omnia_index_load(HSParameters params, FILE * fp, void ** userData);
void omnia_index_save(void * userData, FILE * fp);
int omnia_index_supports_dimensions(void * userData, CuboidDimensions dims);
int omnia_index_data_size(void * userData);
int omnia_index_angle_count(void * userData);
int omnia_index_angles_are_equivalent(void * userData, int a1, int a2);
void omnia_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle);
void omnia_index_completed(void * userData);
