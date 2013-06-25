#ifndef __HEURISTICS_H__
#define __HEURISTICS_H__

#include "subproblems/corners/corner_index.h"
#include "subproblems/eo/eo_index.h"

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
        corner_index_angles_are_equivalent,
        corner_index_get_data,
        corner_index_completed
    },
    {
        "eo",
        eo_index_default_arguments,
        eo_index_initialize,
        eo_index_load,
        eo_index_save,
        eo_index_supports_dimensions,
        eo_index_data_size,
        eo_index_angle_count,
        eo_index_angles_are_equivalent,
        eo_index_get_data,
        eo_index_completed
    }
};

#endif