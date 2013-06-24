#include "indexer_arguments.h"
#include "heuristic/heuristic.h"
#include "algebra/rotation_cosets.h"

typedef struct {
    Heuristic * heuristic;
    Cuboid ** invTriggers;
} HeuristicIndex;

HeuristicIndex * heuristic_index_create(CLArgumentList * args, IndexerArguments indexArgs,
                                        const char * name);
void heuristic_index_free(HeuristicIndex * index);

int heuristic_index_accepts_node(HeuristicIndex * index, int depth, int idaDepth,
                                 const Cuboid * cb, Cuboid * cache);
int heuristic_index_add_node(HeuristicIndex * index, const Cuboid * cb, Cuboid * cache, int depth);
