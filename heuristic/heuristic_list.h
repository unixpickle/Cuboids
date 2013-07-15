#include "heuristic.h"
#include "heuristic_buffer.h"

typedef struct {
    // the HeuristicList has an array of dataSymmetries.
    // for each entry in this array, the cosets array
    // contains a number from 0 to n-1 where n is the number
    // of data cosets in the heuristic. If the number stored
    // is -1, then that symmetry belongs to a coset of the 
    // moveset symmetries and does not belong as a data coset.
    int8_t * cosets;
} HeuristicCosetMap;

typedef struct {
    Heuristic ** heuristics;
    char ** fileNames;
    int count;
    
    // made to enclose the data symmetries of all heuristics
    RotationBasis dataBasis;
    RotationGroup * dataSymmetries;
    HeuristicCosetMap * cosetMaps;
} HeuristicList;

HeuristicList * heuristic_list_new();
void heuristic_list_free(HeuristicList * list);
void heuristic_list_add(HeuristicList * list, Heuristic * h, const char * file);

// called when all heuristics have been added
void heuristic_list_prepare(HeuristicList * list, Cuboid * cache);
int heuristic_list_pruning_value(HeuristicList * list, const Cuboid * cuboid,
                                 Cuboid * cache);
int heuristic_list_exceeds(HeuristicList * list, const Cuboid * cuboid,
                           Cuboid * cache, int maxValue);

