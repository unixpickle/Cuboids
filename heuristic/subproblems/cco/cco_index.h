/**
 * Records the *orientation* of the edges and the centers.
 * That is, this records which centers belong to an axis and also
 * how the corners are oriented with respect to this axis.
 */

#include "heuristic/subproblem_type.h"
#include "arguments/arguments.h"
#include "saving/save_cuboid.h"
#include "saving/save_tools.h"
#include "pieces/co.h"
#include "pieces/ceo.h"

CLArgumentList * cco_index_default_arguments();
int cco_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData);
int cco_index_load(HSParameters params, FILE * fp, void ** userData);
void cco_index_save(void * userData, FILE * fp);
int cco_index_supports_dimensions(void * userData, CuboidDimensions dims);
int cco_index_data_size(void * userData);
int cco_index_angle_count(void * userData);
int cco_index_angles_are_equivalent(void * userData, int a1, int a2);
void cco_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle);
void cco_index_completed(void * userData);
RotationBasis cco_index_data_symmetries(void * userData);
