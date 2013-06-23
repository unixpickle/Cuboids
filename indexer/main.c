#include "indexer_arguments.h"
#include "arguments/parser.h"
#include "heuristic/heuristic_io.h"
#include "search/cuboid.h"
#include "algebra/rotation_cosets.h"
#include <stdio.h>

static Heuristic * heuristic = NULL;
static IndexerArguments arguments;
static Cuboid ** inverseTriggers = NULL;
static CSSearchContext * searchContext;
static pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;
static int currentDepth = 0;
static volatile long long nodesAdded = 0;
static const char * fileName;

void print_usage(const char * name);
CLArgumentList * process_arguments(int argc, const char * argv[]);
HSParameters heuristic_parameters(IndexerArguments args);

int generate_heuristic(const char * name, CLArgumentList * args);
int run_search();

CSCallbacks generate_callbacks();

void indexer_handle_progress(void * data);
void indexer_handle_depth(void * data, int len);
int indexer_accepts_sequence(void * data, const int * sequence, int len, int depthRem);
int indexer_accepts_cuboid(void * data, const Cuboid * cuboid, Cuboid * cache, int depthRem);
void indexer_handle_cuboid(void * data, const Cuboid * cuboid, Cuboid * cache,
                           const int * sequence, int len);
void indexer_handle_save_data(void * data, CSSearchState * save);
void indexer_handle_finished(void * data);

int main(int argc, const char * argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }
    fileName = argv[2];
    
    IndexerArguments args;
    CLArgumentList * list = process_arguments(argc - 3, &argv[3]);
    if (!list) return 1;
    if (!indexer_process_arguments(list, &arguments)) {
        fprintf(stderr, "error: failed to process arguments.\n");
        cl_argument_list_free(list);
        return 1;
    }
    
    indexer_remove_all_defaults(list);
    int result = generate_heuristic(argv[1], list);
    cl_argument_list_free(list);
    if (!result) {
        alg_list_release(arguments.operations);
        fprintf(stderr, "error: failed to create heuristic.\n");
        return 1;
    }
    result = run_search();
    if (!result) {
        fprintf(stderr, "error: failed to launch search.\n");
        alg_list_release(arguments.operations);
        heuristic_free(heuristic);
        return 1;
    }
    return 0;
}

void print_usage(const char * name) {
    fprintf(stderr, "Usage: %s <index type> <output> [--maxdepth=n] [--threads=n]\n\
       [--operations ...] [--symmetries xyz]\n\n", name);
    fflush(stderr);
}

CLArgumentList * process_arguments(int argc, const char * argv[]) {
    CLArgumentList * defaults = indexer_default_arguments();
    int failIndex;
    CLArgumentList * result = cl_parse_arguments(argv, argc, defaults, &failIndex);
    cl_argument_list_free(defaults);
    if (!result) {
        fprintf(stderr, "error: invalid argument near `%s`\n", argv[failIndex]);
    }
    return result;
}

HSParameters heuristic_parameters(IndexerArguments args) {
    HSParameters params;
    params.symmetries = args.symmetries;
    params.maxDepth = args.maxDepth;
    return params;
}

/*********************
 * Commencing action *
 *********************/

int generate_heuristic(const char * name, CLArgumentList * args) {
    HSParameters params = heuristic_parameters(arguments);
    heuristic = heuristic_create(params, args, name);
    if (!heuristic) return 0;
    
    int dataSize = heuristic->subproblem.data_size(heuristic->spUserData);
    int nodeDepth = dataSize < 4 ? dataSize : 4;
    
    // generate cosets
    RotationGroup * symmetries = heuristic->symmetries;
    RotationBasis basis = rotation_basis_standard(heuristic->params.symmetries.dims);
    RotationGroup * group = rotation_group_create_basis(basis);
    RotationCosets * cosets = rotation_cosets_create(group, symmetries);
    
    int cosetCount = rotation_cosets_count(cosets);
    inverseTriggers = (Cuboid **)malloc(sizeof(void *) * cosetCount);
    
    int i;
    for (i = 0; i < cosetCount; i++) {
        DataList * dl = data_list_create(dataSize, 2, nodeDepth);
        heuristic_add_coset(heuristic, dl);
        Cuboid * cuboid = rotation_cosets_get_trigger(cosets, i);
        Cuboid * inv = cuboid_inverse(cuboid);
        inverseTriggers[i] = inv;
    }
    
    rotation_group_release(group);
    rotation_cosets_release(cosets);
    return 1;
}

int run_search() {
    CSCallbacks cbs = generate_callbacks();
    CSSettings settings;
    settings.rootNode = cuboid_create(arguments.symmetries.dims);
    settings.algorithms = arguments.operations;
    settings.cacheCuboid = 1;
    BSSettings bsSettings;
    bsSettings.threadCount = arguments.threadCount;
    bsSettings.minDepth = 0;
    bsSettings.maxDepth = arguments.maxDepth;
    bsSettings.nodeInterval = 1000000;
    
    searchContext = cs_run(settings, bsSettings, cbs);
    
    while (1) {
        sleep(1);
    }
    
    return 1;
}

/*************
 * Callbacks *
 *************/

CSCallbacks generate_callbacks() {
    CSCallbacks cbs;
    cbs.handle_progress = indexer_handle_progress;
    cbs.handle_depth = indexer_handle_depth;
    cbs.accepts_sequence = indexer_accepts_sequence;
    cbs.accepts_cuboid = indexer_accepts_cuboid;
    cbs.handle_cuboid = indexer_handle_cuboid;
    cbs.handle_save_data = indexer_handle_save_data;
    cbs.handle_finished = indexer_handle_finished;
    return cbs;
}

void indexer_handle_progress(void * data) {
    pthread_mutex_lock(&globalMutex);
    long long value = __sync_fetch_and_or(&nodesAdded, 0);
    printf("Expanded %lld nodes [depth %d, cosetCount = %d].\n", 
           value, currentDepth, heuristic->cosetCount);
    pthread_mutex_unlock(&globalMutex);
}

void indexer_handle_depth(void * data, int len) {
    pthread_mutex_lock(&globalMutex);
    printf("Exploring depth of %d.\n", len);
    currentDepth = len;
    pthread_mutex_unlock(&globalMutex);
}

int indexer_accepts_sequence(void * data, const int * sequence, int len, int depthRem) {
    return 1;
}

int indexer_accepts_cuboid(void * data, const Cuboid * cuboid, Cuboid * cache, int depthRem) {
    // compare this cuboid to our first coset for pruning...
    cuboid_multiply(cache, cuboid, inverseTriggers[0]);
    int value = heuristic_coset_value(heuristic, cache, 0, 0);
    if (value < 0) return 1;
    if (value < currentDepth - depthRem) {
        return 0;
    }
    return 1;
}

void indexer_handle_cuboid(void * data, const Cuboid * cuboid, Cuboid * cache,
                           const int * sequence, int len) {
    uint8_t lenData[2] = {len & 0xff, (len >> 8) & 0xff};
        
    int dataSize = heuristic->subproblem.data_size(heuristic->spUserData);
    uint8_t * heuristicData = (uint8_t *)malloc(dataSize);
    Cuboid * temp = cuboid_create(cuboid->dimensions);
    
    int i, addedSomething = 0;
    for (i = 0; i < heuristic->cosetCount; i++) {
        cuboid_multiply(temp, cuboid, inverseTriggers[i]);
        DataList * dl = heuristic->cosets[i];
        heuristic->subproblem.get_data(heuristic->spUserData, temp,
                                       heuristicData, 0);
        pthread_mutex_lock(&globalMutex);
        DataListNode * base = data_list_find_base(dl, heuristicData, 1);
        if (data_list_base_add(base, heuristicData, lenData)) {
            addedSomething = 1;
        }
        pthread_mutex_unlock(&globalMutex);
    }
    
    if (addedSomething) {
        __sync_fetch_and_add(&nodesAdded, 1);
    }
    cuboid_free(temp);
    free(heuristicData);
}

void indexer_handle_save_data(void * data, CSSearchState * save) {
    puts("handle_save_data should never be called!");
    abort();
}

void indexer_handle_finished(void * data) {
    cs_context_release(searchContext);
    // free inverse triggers
    int i;
    for (i = 0; i < heuristic->cosetCount; i++) {
        cuboid_free(inverseTriggers[i]);
    }
    free(inverseTriggers);
    
    puts("Writing to output file...");
    FILE * fp = fopen(fileName, "w");
    save_heuristic(heuristic, fp);
    fclose(fp);
    heuristic_free(heuristic);
    
    exit(0);
}
