#include "cuboid.h"

static CSSearchContext * _cs_search_context_create(CSSettings s, BSSettings bs, CSCallbacks c);
static void _cs_search_context_free(CSSearchContext * context);
static BSCallbacks _cs_standard_bs_callbacks(void * data);

static void _cs_handle_reached(void * data, const int * sequence, int depth, int th);
static void _cs_handle_depth_increase(void * data, int depth);
static int _cs_should_expand(void * data, const int * sequence, int len, int depth, int th);
static void _cs_handle_save_data(void * data, void * save);
static void _cs_handle_progress_update(void * data);
static void _cs_handle_search_complete(void * data);

CSSearchContext * cs_run(CSSettings settings, BSSettings bsSettings, CSCallbacks callbacks) {
    bsSettings.operationCount = settings.algorithms->entryCount;
    
    CSSearchContext * context = _cs_search_context_create(settings, bsSettings, callbacks);
    BSCallbacks cbs = _cs_standard_bs_callbacks(context);
    
    context->startTime = time(NULL);
    context->bsContext = bs_run(bsSettings, cbs);
    
    return context;
}

CSSearchContext * cs_resume(CSSearchState * state, CSCallbacks callbacks) {
    BSSettings bsSettings = state->bsState->settings;
    CSSettings settings = state->settings;
    CSSearchContext * context = _cs_search_context_create(settings, bsSettings, callbacks);
    BSCallbacks cbs = _cs_standard_bs_callbacks(context);
    
    context->startTime = time(NULL);
    context->bsContext = bs_resume(state->bsState, cbs);
    
    free(state);
    return context;
}

/*************************
 * Cuboid search context *
 *************************/

void cs_context_release(CSSearchContext * context) {
    pthread_mutex_lock(&context->mutex);
    
    context->retainCount--;
    if (context->retainCount == 0) {
        pthread_mutex_unlock(&context->mutex);
        _cs_search_context_free(context);
        return;
    }
    
    pthread_mutex_unlock(&context->mutex);
}

void cs_context_retain(CSSearchContext * context) {
    pthread_mutex_lock(&context->mutex);
    context->retainCount++;
    pthread_mutex_unlock(&context->mutex);
}

int cs_context_is_running(CSSearchContext * context) {
    return !bs_context_is_stopped(context->bsContext);
}

void cs_context_stop(CSSearchContext * context, int save) {
    bs_context_stop(context->bsContext, save);
    context->isStopping = 1;
}

void cs_search_state_free(CSSearchState * state) {
    bs_search_state_free(state->bsState);
    CSSettings settings = state->settings;
    cuboid_free(settings.rootNode);
    alg_list_release(settings.algorithms);
    
    free(state);
}

/***********
 * Private *
 ***********/

static CSSearchContext * _cs_search_context_create(CSSettings s, BSSettings bs, CSCallbacks c) {
    CSSearchContext * context = (CSSearchContext *)malloc(sizeof(CSSearchContext));
    bzero(context, sizeof(CSSearchContext));
    
    // allocate the cache
    int tc = bs.threadCount, i;
    context->caches = (SequenceCache **)malloc(sizeof(SequenceCache *) * tc);
    for (i = 0; i < tc; i++) {
        context->caches[i] = sequence_cache_create(s.rootNode, s.cacheCuboid);
    }
    
    context->settings = s;
    context->callbacks = c;
    pthread_mutex_init(&context->mutex, NULL);
    context->retainCount = 2;
    
    return context;
}

static void _cs_search_context_free(CSSearchContext * context) {
    int i;
    int tc = context->bsContext->settings.threadCount;
    for (i = 0; i < tc; i++) {
        sequence_cache_free(context->caches[i]);
    }
    free(context->caches);
    pthread_mutex_destroy(&context->mutex);
    
    bs_context_release(context->bsContext);
    
    // free the settings
    CSSettings settings = context->settings;
    cuboid_free(settings.rootNode);
    alg_list_release(settings.algorithms);
    
    free(context);
}

static BSCallbacks _cs_standard_bs_callbacks(void * data) {
    BSCallbacks cbs;
    cbs.userData = data;
    cbs.handle_reached_node = _cs_handle_reached;
    cbs.handle_depth_increase = _cs_handle_depth_increase;
    cbs.should_expand = _cs_should_expand;
    cbs.handle_save_data = _cs_handle_save_data;
    cbs.handle_progress_update = _cs_handle_progress_update;
    cbs.handle_search_complete = _cs_handle_search_complete;
    return cbs;
}

/******************
 * Base Callbacks *
 ******************/

static void _cs_handle_reached(void * data, const int * sequence, int depth, int th) {
    CSSearchContext * ctx = (CSSearchContext *)data;
    if (ctx->isStopping) return;
    
    assert(th < ctx->bsContext->settings.threadCount);
    SequenceCache * cache = ctx->caches[th];
    const Cuboid * useCuboid = sequence_cache_make_cuboid(cache,
                                                          ctx->settings.algorithms,
                                                          sequence, depth);
    CSCallbacks cb = ctx->callbacks;
    if (cb.handle_cuboid) {
        cb.handle_cuboid(cb.userData, useCuboid, cache->userCache,
                         sequence, depth);
    }
}

static void _cs_handle_depth_increase(void * data, int depth) {
    // NOTE: this DEPENDS on the fact that the base search blocks until
    // this function returns
    
    CSSearchContext * ctx = (CSSearchContext *)data;
    
    // clear all caches
    int i, threadCount = ctx->bsContext->settings.threadCount;
    for (i = 0; i < threadCount; i++) {
        sequence_cache_clear(ctx->caches[i]);
    }
    
    CSCallbacks cb = ctx->callbacks;
    if (cb.handle_depth) {
        cb.handle_depth(cb.userData, depth);
    }
}

static int _cs_should_expand(void * data, const int * sequence, int len, int depth, int th) {
    if (len == 0) return 1;
    
    CSSearchContext * ctx = (CSSearchContext *)data;
    assert(th < ctx->bsContext->settings.threadCount);
    
    // filter the sequence
    CSCallbacks cb = ctx->callbacks;
    if (cb.accepts_sequence) {
        if (!cb.accepts_sequence(cb.userData, sequence, len, depth - len)) {
            return 0;
        }
    }
    
    SequenceCache * cache = ctx->caches[th];
    const Cuboid * cuboid = sequence_cache_make_cuboid(cache, ctx->settings.algorithms,
                                                       sequence, len);
    if (cb.accepts_cuboid) {
        if (!cb.accepts_cuboid(cb.userData, cuboid, cache->userCache, depth - len)) {
            return 0;
        }
    }
    
    return 1;
}

static void _cs_handle_save_data(void * data, void * save) {
    CSSearchContext * ctx = (CSSearchContext *)data;
    if (!ctx->callbacks.handle_save_data) return;
    
    BSSearchState * bsState = (BSSearchState *)save;
    
    CSSearchState * state = (CSSearchState *)malloc(sizeof(CSSearchState));
    CSSettings settings = ctx->settings;
    alg_list_retain(settings.algorithms);
    settings.rootNode = cuboid_copy(settings.rootNode);
    state->bsState = bsState;
    state->settings = settings;
    
    ctx->callbacks.handle_save_data(ctx->callbacks.userData, state);
}

static void _cs_handle_progress_update(void * data) {
    CSSearchContext * ctx = (CSSearchContext *)data;
    CSCallbacks callbacks = ctx->callbacks;
    if (callbacks.handle_progress) {
        callbacks.handle_progress(callbacks.userData);
    }
}

static void _cs_handle_search_complete(void * data) {
    CSSearchContext * ctx = (CSSearchContext *)data;
    CSCallbacks callbacks = ctx->callbacks;
    if (callbacks.handle_finished) {
        callbacks.handle_finished(callbacks.userData);
    }
    
    cs_context_release(ctx);
}
