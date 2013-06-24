#include "subproblem_type.h"

typedef struct {
    int numAngles;
    int numDistinct;
    int * distinct; // count = numDistinct
    int * saveAngles; // count = numAngles
} HeuristicAngles;

HeuristicAngles * heuristic_angles_new();
void heuristic_angles_add_distinct(HeuristicAngles * angles);
void heuristic_angles_add_duplicate(HeuristicAngles * angles, int saveAngle);
void heuristic_angles_free(HeuristicAngles * angles);

HeuristicAngles * heuristic_angles_for_subproblem(HSubproblem sp, void * userData);
int heuristic_angles_equal(HeuristicAngles * a1, HeuristicAngles * a2);
