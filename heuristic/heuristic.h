#include "subproblem_table.h"
#include "data_list.h"

typedef struct {
    HSubproblem subproblem;
    void * spUserData;
    HSParameters params;
    
    int cosetCount;
    DataList ** cosets;
    
    RotationGroup * symmetries;
} Heuristic;

Heuristic * heuristic_create(HSParameters params, CLArgumentList * args, const char * spName);
void heuristic_free(Heuristic * heuristic);

void heuristic_add_coset(Heuristic * heuristic, DataList * coset);

// applies each symmetry and checks each coset table to get the lowest
// pruning value
int heuristic_pruning_value(Heuristic * heuristic, const Cuboid * cuboid, Cuboid * scratchpad);
