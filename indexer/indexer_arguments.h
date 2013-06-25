#include "algebra/rotation_group.h"
#include "arguments/search_args.h"
#include "algebra/basis.h"

typedef struct {
    RotationBasis symmetries;
    int maxDepth;
    int shardDepth;
    int threadCount;
    AlgList * operations;
} IndexerArguments;

CLArgumentList * indexer_default_arguments();
int indexer_process_arguments(CLArgumentList * args, IndexerArguments * out);
void indexer_remove_all_defaults(CLArgumentList * args);
