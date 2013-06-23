#include "arguments/search_args.h"
#include "algebra/rotation_group.h"

typedef struct {
    RotationBasis symmetries;
    CLSearchParameters search;
} IndexerArguments;

CLArgumentList * indexer_default_arguments();
int indexer_process_arguments(CLArgumentList * args, IndexerArguments * out);
