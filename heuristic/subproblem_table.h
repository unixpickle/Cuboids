#ifndef __HEURISTICS_H__
#define __HEURISTICS_H__

#include "subproblems/corners/corner_index.h"
#include "subproblems/eo/eo_index.h"
#include "subproblems/dedges/dedge_index.h"
#include "subproblems/omnia/omnia_index.h"
#include "subproblems/centers/center_index.h"
#include "subproblems/cco/cco_index.h"
#include "subproblems/dedgepair/dedgepair_index.h"
#include "subproblems/centergroup/centergroup_index.h"

const static HSubproblem HSubproblemTable[] = {
    {
        "corners", "the physical corners",
        corner_index_default_arguments,
        corner_index_initialize,
        corner_index_load,
        corner_index_save,
        corner_index_supports_dimensions,
        corner_index_data_size,
        corner_index_angle_count,
        corner_index_angles_are_equivalent,
        corner_index_get_data,
        corner_index_completed,
        NULL
    },
    {
        "eo", "edge orientations along three axes",
        eo_index_default_arguments,
        eo_index_initialize,
        eo_index_load,
        eo_index_save,
        eo_index_supports_dimensions,
        eo_index_data_size,
        eo_index_angle_count,
        eo_index_angles_are_equivalent,
        eo_index_get_data,
        eo_index_completed,
        eo_index_data_symmetries
    },
    {
        "dedges", "a set of physical dedges",
        dedge_index_default_arguments,
        dedge_index_initialize,
        dedge_index_load,
        dedge_index_save,
        dedge_index_supports_dimensions,
        dedge_index_data_size,
        dedge_index_angle_count,
        dedge_index_angles_are_equivalent,
        dedge_index_get_data,
        dedge_index_completed,
        NULL
    },
    {
        "omnia", "an index for everything",
        omnia_index_default_arguments,
        omnia_index_initialize,
        omnia_index_load,
        omnia_index_save,
        omnia_index_supports_dimensions,
        omnia_index_data_size,
        omnia_index_angle_count,
        omnia_index_angles_are_equivalent,
        omnia_index_get_data,
        omnia_index_completed,
        NULL
    },
    {
        "centers", "indexes center pieces on selected faces",
        center_index_default_arguments,
        center_index_initialize,
        center_index_load,
        center_index_save,
        center_index_supports_dimensions,
        center_index_data_size,
        center_index_angle_count,
        center_index_angles_are_equivalent,
        center_index_get_data,
        center_index_completed,
        NULL
    },
    {
        "cco", "corner and center \"orientations\" along three axes",
        cco_index_default_arguments,
        cco_index_initialize,
        cco_index_load,
        cco_index_save,
        cco_index_supports_dimensions,
        cco_index_data_size,
        cco_index_angle_count,
        cco_index_angles_are_equivalent,
        cco_index_get_data,
        cco_index_completed,
        cco_index_data_symmetries
    },
    {
        "dedgepair", "compact information about edge pairing",
        dedgepair_index_default_arguments,
        dedgepair_index_initialize,
        dedgepair_index_load,
        dedgepair_index_save,
        dedgepair_index_supports_dimensions,
        dedgepair_index_data_size,
        dedgepair_index_angle_count,
        dedgepair_index_angles_are_equivalent,
        dedgepair_index_get_data,
        dedgepair_index_completed,
        dedgepair_index_data_symmetries
    },
    {
        "centergroup", "compact information about center grouping",
        centergroup_index_default_arguments,
        centergroup_index_initialize,
        centergroup_index_load,
        centergroup_index_save,
        centergroup_index_supports_dimensions,
        centergroup_index_data_size,
        centergroup_index_angle_count,
        centergroup_index_angles_are_equivalent,
        centergroup_index_get_data,
        centergroup_index_completed,
        centergroup_index_data_symmetries
    }
};

#endif