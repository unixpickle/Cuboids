#include "heuristic/subproblem_type.h"
#include "arguments/arguments.h"

CLArgumentList * corner_index_default_arguments();
int corner_index_initialize(HSRunParameters params, CLArgumentList * arguments, void ** userData);
int corner_index_load(HSSaveParameters params, FILE * fp, void ** userData);
void corner_index_save(void * userData, FILE * fp);
int corner_index_supports_dimensions(void * userData, CuboidDimensions dims);
int corner_index_data_size(void * userData);
int corner_index_angle_count(void * userData);
void corner_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle);
void corner_index_completed(void * userData);

static const HSubproblem CornerIndexSubproblem = {
    "corners",
    corner_index_default_arguments,
    corner_index_initialize,
    corner_index_load,
    corner_index_save,
    corner_index_supports_dimensions,
    corner_index_data_size,
    corner_index_angle_count,
    corner_index_get_data,
    corner_index_completed
};
