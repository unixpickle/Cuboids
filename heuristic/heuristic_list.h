#include "heuristic.h"

typedef struct {
    Heuristic ** heuristics;
    char ** fileNames;
    int count;
} HeuristicList;

HeuristicList * heuristic_list_new();
void heuristic_list_free(HeuristicList * list);
void heuristic_list_add(HeuristicList * list, Heuristic * h, const char * file);
