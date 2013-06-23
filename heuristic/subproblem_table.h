#ifndef __HEURISTICS_H__
#define __HEURISTICS_H__

#include "subproblems/corners/corner_index.h"

const static HSubproblem HSubproblemTable[] = {
    {
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
    }
};

#endif