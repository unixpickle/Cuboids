#include "save_search.h"

static void _save_bs_settings(BSSettings settings, FILE * fp);
static void _save_bs_progress(BSProgress progress, FILE * fp);
static void _save_s_boundary(SBoundary bound, FILE * fp);
static void _save_cs_settings(CSSettings settings, FILE * fp);

static int _load_bs_threads(BSSearchState * state, FILE * fp);
static int _load_bs_settings(BSSettings * settings, FILE * fp);
static int _load_bs_progress(BSProgress * progress, FILE * fp);
static int _load_s_boundary(SBoundary * boundary, FILE * fp);
static int _load_cs_settings(CSSettings * settings, FILE * fp);

void save_cuboid_search(CSSearchState * state, FILE * fp) {
    _save_cs_settings(state->settings, fp);
    save_base_search(state->bsState, fp);
}

void save_base_search(BSSearchState * state, FILE * fp) {
    uint32_t threadCount = state->threadCount;
    uint32_t depth = state->depth;
    save_uint32(depth, fp);
    
    _save_bs_settings(state->settings, fp);
    _save_bs_progress(state->progress, fp);
    
    int i;
    save_uint32(threadCount, fp);
    for (i = 0; i < threadCount; i++) {
        BSThreadState * th = state->states[i];
        _save_s_boundary(th->range.lower, fp);
        _save_s_boundary(th->range.upper, fp);
    }
}

CSSearchState * load_cuboid_search(FILE * fp) {
    CSSettings settings;
    if (!_load_cs_settings(&settings, fp)) return 0;
    
    BSSearchState * state = load_base_search(fp);
    if (!state) {
        cuboid_free(settings.rootNode);
        alg_list_release(settings.algorithms);
        return NULL;
    }
    
    CSSearchState * result = (CSSearchState *)malloc(sizeof(CSSearchState));
    result->settings = settings;
    result->bsState = state;
    
    return result;
}

BSSearchState * load_base_search(FILE * fp) {
    uint32_t depth;
    if (!load_uint32(&depth, fp)) return NULL;
    
    BSSearchState * state = (BSSearchState *)malloc(sizeof(BSSearchState));
    state->depth = depth;
    
    if (!_load_bs_settings(&state->settings, fp)) {
        free(state);
        return NULL;
    }
    
    if (!_load_bs_progress(&state->progress, fp)) {
        free(state);
        return NULL;
    }
            
    if (!_load_bs_threads(state, fp)) {
        free(state);
        return NULL;
    }
        
    return state;
}

/**********
 * Saving *
 **********/

static void _save_bs_settings(BSSettings settings, FILE * fp) {
    uint32_t buffer;
    
    buffer = settings.operationCount;
    save_uint32(buffer, fp);
        
    buffer = settings.threadCount;
    save_uint32(buffer, fp);
    
    buffer = settings.minDepth;
    save_uint32(buffer, fp);
    
    buffer = settings.maxDepth;
    save_uint32(buffer, fp);
    
    buffer = settings.nodeInterval;
    save_uint32(buffer, fp);
}

static void _save_bs_progress(BSProgress progress, FILE * fp) {
    uint64_t buffer;
    
    buffer = progress.nodesExpanded;
    save_uint64(buffer, fp);
    
    buffer = progress.nodesPruned;
    save_uint64(buffer, fp);
}

static void _save_s_boundary(SBoundary bound, FILE * fp) {
    uint32_t buffer;
    
    buffer = bound.length;
    save_uint32(buffer, fp);
    buffer = bound.base;
    save_uint32(buffer, fp);
    
    int i;
    for (i = 0; i < bound.length; i++) {
        buffer = (uint32_t)bound.sequence[i];
        save_uint32(buffer, fp);
    }
}

static void _save_cs_settings(CSSettings settings, FILE * fp) {
    uint8_t cache = settings.cacheCuboid;
    fwrite(&cache, 1, 1, fp);
    save_cuboid(settings.rootNode, fp);
    save_alg_list(settings.algorithms, fp);
}

/***********
 * Loading *
 ***********/

static int _load_bs_threads(BSSearchState * state, FILE * fp) {
    uint32_t threadCount;
    if (!load_uint32(&threadCount, fp)) return 0;
    state->states = (BSThreadState **)malloc(sizeof(void *) * threadCount + 1);
    state->threadCount = threadCount;
    int i, j;
    for (i = 0; i < threadCount; i++) {
        BSThreadState * th = (BSThreadState *)malloc(sizeof(BSThreadState));
        int failed = 1;
        if (_load_s_boundary(&th->range.lower, fp)) {
            if (_load_s_boundary(&th->range.upper, fp)) {
                failed = 0;
            } else {
                sboundary_destroy(th->range.lower);
            }
        }
        if (failed) {
            free(th);
            for (j = 0; j < i; j++) {
                BSThreadState * freeMe = state->states[j];
                sboundary_destroy(freeMe->range.lower);
                sboundary_destroy(freeMe->range.upper);
                free(freeMe);
            }
            free(state->states);
            return 0;
        } else {
            state->states[i] = th;
        }
    }
    return 1;
}

static int _load_bs_settings(BSSettings * settings, FILE * fp) {
    uint32_t buffer;
    
    if (!load_uint32(&buffer, fp)) return 0;
    settings->operationCount = buffer;
        
    if (!load_uint32(&buffer, fp)) return 0;
    settings->threadCount = buffer;
    
    if (!load_uint32(&buffer, fp)) return 0;
    settings->minDepth = buffer;
    
    if (!load_uint32(&buffer, fp)) return 0;
    settings->maxDepth = buffer;
    
    if (!load_uint32(&buffer, fp)) return 0;
    settings->nodeInterval = buffer;
    
    return 1;
}

static int _load_bs_progress(BSProgress * progress, FILE * fp) {
    uint64_t buffer;
    
    if (!load_uint64(&buffer, fp)) return 0;
    progress->nodesExpanded = buffer;
    
    if (!load_uint64(&buffer, fp)) return 0;
    progress->nodesPruned = buffer;
    
    return 1;
}

static int _load_s_boundary(SBoundary * boundary, FILE * fp) {
    uint32_t length, base;
    if (!load_uint32(&length, fp)) return 0;
    if (!load_uint32(&base, fp)) return 0;
    sboundary_initialize(boundary, (int32_t)length, (int32_t)base);
    int i;
    for (i = 0; i < length; i++) {
        uint32_t num;
        if (!load_uint32(&num, fp)) {
            sboundary_destroy(*boundary);
            return 0;
        }
        boundary->sequence[i] = (int32_t)num;
    }
    return 1;
}

static int _load_cs_settings(CSSettings * settings, FILE * fp) {
    uint8_t cache;
    if (fread(&cache, 1, 1, fp) != 1) return 0;
    
    Cuboid * c = load_cuboid(fp);
    if (!c) return 0;
    
    AlgList * list = load_alg_list(fp);
    if (!list) {
        cuboid_free(c);
        return 0;
    }
    
    settings->cacheCuboid = cache;
    settings->rootNode = c;
    settings->algorithms = list;
    return 1;
}
