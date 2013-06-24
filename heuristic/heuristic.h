#ifndef __HEURISTIC_H__
#define __HEURISTIC_H__

#include "subproblem_table.h"
#include "data_list.h"
#include "heuristic_angles.h"

typedef struct {
    HSubproblem subproblem;
    void * spUserData;
    HSParameters params;
    
    int cosetCount;
    DataList ** cosets;
    
    RotationGroup * symmetries;
    HeuristicAngles * angles;
} Heuristic;

Heuristic * heuristic_create(HSParameters params, CLArgumentList * args, const char * spName);
void heuristic_free(Heuristic * heuristic);

/* low level heuristic functions */

// may include an extra byte for the angle index...
int heuristic_data_size(Heuristic * heuristic);
void heuristic_add_coset(Heuristic * heuristic, DataList * coset);
void heuristic_get_data(Heuristic * heuristic, const Cuboid * cuboid,
                        int angle, uint8_t * dataOut);

/* user-friendly functions */
                                
// applies each symmetry and checks each coset for every angle.
// picks the highest angle value and the lowest symmetry value for each angle.
int heuristic_pruning_value(Heuristic * heuristic, const Cuboid * cuboid, Cuboid * scratchpad);

#endif