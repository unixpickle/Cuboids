#include "heuristic/heuristic.h"

CLArgumentList * corner_index_default_arguments();
int corner_index_run(CLSearchParameters * params, CLArgumentList * arguments, void ** userData);
int corner_index_load(CLSearchParameters * params, FILE * fp, void ** userData);
int corner_index_data_size(void * userData);
int corner_index_angle_count(void * userData);
void corner_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle);
void corner_index_completed(void * userData);

static const Heuristic CornerIndexHeurstic = {
    corner_index_run,
    corner_index_load,
    corner_index_data_size,
    corner_index_angle_count,
    corner_index_get_data,
    corner_index_completed
};
