#ifndef __HEURISTICS_H__
#define __HEURISTICS_H__

#include "subproblems/corners/corner_index.h"
#include "subproblems/eo/eo_index.h"
#include "subproblems/dedges/dedge_index.h"
#include "subproblems/omnia/omnia_index.h"
#include "subproblems/centers/center_index.h"

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
    },
    {
        "dedges",
        dedge_index_default_arguments,
        dedge_index_initialize,
        dedge_index_load,
        dedge_index_save,
        dedge_index_supports_dimensions,
        dedge_index_data_size,
        dedge_index_angle_count,
        dedge_index_angles_are_equivalent,
        dedge_index_get_data,
        dedge_index_completed
    },
    {
        "omnia",
        omnia_index_default_arguments,
        omnia_index_initialize,
        omnia_index_load,
        omnia_index_save,
        omnia_index_supports_dimensions,
        omnia_index_data_size,
        omnia_index_angle_count,
        omnia_index_angles_are_equivalent,
        omnia_index_get_data,
        omnia_index_completed
    },
    {
        "centers",
        center_index_default_arguments,
        center_index_initialize,
        center_index_load,
        center_index_save,
        center_index_supports_dimensions,
        center_index_data_size,
        center_index_angle_count,
        center_index_angles_are_equivalent,
        center_index_get_data,
        center_index_completed
    }
};

#endif