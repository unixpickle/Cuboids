#include "base.h"

typedef struct {
    SRange range;
    BSSearchContext * context;
    
    unsigned long long nodeCount;
    unsigned long long pruneCount;
    unsigned int threadIndex;
    
    int * sequence;
    int currentDepth;
    int depth;
    
    time_t lastUpdate;
} BSThreadContext;

static BSThreadContext * _bs_thread_context_create(SRange range, int depth, BSSearchContext * context);
static BSThreadContext * _bs_thread_context_load(BSThreadState * save, int depth, BSSearchContext * ctx);
static BSThreadState * _bs_thread_context_save(BSThreadContext * context);
static void _bs_thread_context_free(BSThreadContext * context);

static BSSearchState * _bs_search_state_create(BSSearchContext * context);
static void _bs_search_state_add_thread(BSSearchState * state, BSThreadState * thread);

static void * _bs_run_dispatch(void * context);
static void * _bs_resume_dispatch(void * context);

static void * _bs_search_thread(void * threadContext);
static int _bs_recursive_search(BSThreadContext * context);
static int _bs_recursive_search_hit_base(BSThreadContext * context);
static int _bs_recursive_search_progress_update(BSThreadContext * context);

BSSearchContext * bs_run(BSSettings settings, BSCallbacks callbacks) {
    BSSearchContext * context = (BSSearchContext *)malloc(sizeof(BSSearchContext));
    bzero(context, sizeof(BSSearchContext));
    context->retainCount = 2;
    context->settings = settings;
    context->callbacks = callbacks;
    context->currentDepth = settings.minDepth;
    context->isRunning = 1;
    
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&context->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    
    pthread_create(&context->dispatchThread, NULL, &_bs_run_dispatch, context);
    
    return context;
}

BSSearchContext * bs_resume(BSSearchState * state, BSCallbacks callbacks) {
    BSSearchContext * context = (BSSearchContext *)malloc(sizeof(BSSearchContext));
    bzero(context, sizeof(BSSearchContext));
    context->retainCount = 2;
    context->settings = state->settings;
    context->callbacks = callbacks;
    context->saveData = state;
    context->currentDepth = state->depth;
    context->progress = state->progress;
    context->isRunning = 1;
    
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&context->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    
    pthread_create(&context->dispatchThread, NULL, &_bs_resume_dispatch, context);
    
    return context;
}

/************************************
 * Public BSSearchContext functions *
 ************************************/

void bs_context_retain(BSSearchContext * context) {
    pthread_mutex_lock(&context->mutex);
    context->retainCount++;
    pthread_mutex_unlock(&context->mutex);
}

void bs_context_release(BSSearchContext * context) {
    pthread_mutex_lock(&context->mutex);
    context->retainCount--;
    if (context->retainCount == 0) {
        pthread_mutex_unlock(&context->mutex);
        pthread_mutex_destroy(&context->mutex);
        if (context->saveData) {
            bs_search_state_free(context->saveData);
        }
        free(context);
    } else {
        pthread_mutex_unlock(&context->mutex);
    }
}

void bs_context_stop(BSSearchContext * context, int save) {
    pthread_mutex_lock(&context->mutex);
    
    if (!context->isRunning) {
        // the context is already stopped
        pthread_mutex_unlock(&context->mutex);
        return;
    }
    
    context->isRunning = 0;
    context->shouldSave = save;
    
    pthread_mutex_unlock(&context->mutex);
    pthread_join(context->dispatchThread, NULL);
}

BSProgress bs_context_progress(BSSearchContext * context) {
    BSProgress p;
    
    pthread_mutex_lock(&context->mutex);
    p = context->progress;
    pthread_mutex_unlock(&context->mutex);
    
    return p;
}

int bs_context_current_depth(BSSearchContext * context) {
    int d;
    
    pthread_mutex_lock(&context->mutex);
    d = context->currentDepth;
    pthread_mutex_unlock(&context->mutex);
    
    return d;
}

int bs_context_is_stopped(BSSearchContext * context) {
    int s;
    
    pthread_mutex_lock(&context->mutex);
    s = !context->isRunning;
    pthread_mutex_unlock(&context->mutex);
    
    return s;
}

int bs_context_should_save(BSSearchContext * context) {
    int s;
    
    pthread_mutex_lock(&context->mutex);
    s = context->shouldSave;
    pthread_mutex_unlock(&context->mutex);
    
    return s;
}

/****************
 * Search state *
 ****************/

void bs_search_state_free(BSSearchState * state) {
    int i, stateCount = state->threadCount;
    for (i = 0; i < stateCount; i++) {
        BSThreadState * thread = state->states[i];
        sboundary_destroy(thread->range.lower);
        sboundary_destroy(thread->range.upper);
        free(state->states[i]);
    }
    if (state->states) free(state->states);
    free(state);
}

/***********
 * PRIVATE *
 ***********/


/******************
 * Thread context *
 ******************/

static BSThreadContext * _bs_thread_context_create(SRange range, int depth,
                                                   BSSearchContext * context) {
    BSThreadContext * tc = (BSThreadContext *)malloc(sizeof(BSThreadContext));
    bzero(tc, sizeof(BSThreadContext));
    tc->sequence = (int *)malloc(sizeof(int) * depth + 1);
    tc->depth = depth;
    tc->context = context;
    tc->range = range;
    tc->lastUpdate = time(NULL);
    return tc;
}

static BSThreadContext * _bs_thread_context_load(BSThreadState * save, int depth,
                                                 BSSearchContext * ctx) {
    SRange range;
    sboundary_copy(&range.lower, save->range.lower);
    sboundary_copy(&range.upper, save->range.upper);
    
    BSThreadContext * tc = (BSThreadContext *)malloc(sizeof(BSThreadContext));
    bzero(tc, sizeof(BSThreadContext));
    tc->sequence = (int *)malloc(sizeof(int) * depth + 1);
    tc->depth = depth;
    tc->context = ctx;
    tc->range = range;
    tc->lastUpdate = time(NULL);
    return tc;
}

static BSThreadState * _bs_thread_context_save(BSThreadContext * context) {
    assert(context->depth == context->range.upper.length);
    assert(context->depth == context->range.lower.length);
    
    BSThreadState * state = (BSThreadState *)malloc(sizeof(BSThreadState));
    SRange range;
    sboundary_copy(&range.upper, context->range.upper);
    sboundary_copy(&range.lower, context->range.lower);
    
    // calculate the new lower range based on the sequence
    int sequenceHugsLower = 1;
    int i;
    for (i = 0; i < context->currentDepth; i++) {
        assert(context->sequence[i] >= range.lower.sequence[i]);
        if (context->sequence[i] > range.lower.sequence[i]) {
            sequenceHugsLower = 0;
            break;
        }
    }
    if (!sequenceHugsLower) {
        // we have a new lower boundary
        bzero(range.lower.sequence, sizeof(int) * context->depth);
        for (i = 0; i < context->currentDepth; i++) {
            range.lower.sequence[i] = context->sequence[i];
        }
    }
    state->range = range;
    return state;
}

static void _bs_thread_context_free(BSThreadContext * context) {
    free(context->sequence);
    sboundary_destroy(context->range.lower);
    sboundary_destroy(context->range.upper);
    free(context);
}

/**********************
 * Search state stuff *
 **********************/

static BSSearchState * _bs_search_state_create(BSSearchContext * context) {
    BSSearchState * state = (BSSearchState *)malloc(sizeof(BSSearchState));
    bzero(state, sizeof(BSSearchState));
    state->settings = context->settings;
    return state;
}

static void _bs_search_state_add_thread(BSSearchState * state, BSThreadState * thread) {
    if (!state->states) {
        state->states = (BSThreadState **)malloc(sizeof(BSThreadState *));
    } else {
        int newSize = sizeof(BSThreadState *) * (state->threadCount + 1);
        state->states = (BSThreadState **)realloc(state->states, newSize);
    }
    state->states[state->threadCount] = thread;
    state->threadCount++;
}

/*******************
 * Search Dispatch *
 *******************/

static void * _bs_run_dispatch(void * _context) {
    BSSearchContext * context = (BSSearchContext *)_context;
    SRange * ranges = (SRange *)malloc(sizeof(SRange) * context->settings.threadCount);
    pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) * context->settings.threadCount);
    
    BSSearchState * state = _bs_search_state_create(context);
    int depth, i;
    for (depth = context->currentDepth; depth <= context->settings.maxDepth; depth++) {
        pthread_mutex_lock(&context->mutex);
        context->currentDepth = depth;
        pthread_mutex_unlock(&context->mutex);
        context->callbacks.handle_depth_increase(context->callbacks.userData, depth);
        
        // generate the threads and run them
        int count = srange_division(depth, context->settings.operationCount,
                                    context->settings.threadCount, ranges);
        for (i = 0; i < count; i++) {
            SRange range = ranges[i];
            BSThreadContext * tc = _bs_thread_context_create(range, depth, context);
            tc->threadIndex = i;
            pthread_create(&threads[i], NULL, &_bs_search_thread, tc);
        }
        for (i = 0; i < count; i++) {
            void * savedData = NULL;
            pthread_join(threads[i], &savedData);
            if (savedData) {
                _bs_search_state_add_thread(state, (BSThreadState *)savedData);
            }
        }
        state->depth = depth;
        state->progress = context->progress;
        if (bs_context_is_stopped(context)) break;
    }
    
    if (bs_context_should_save(context)) {
        context->callbacks.handle_save_data(context->callbacks.userData,
                                            state);
    } else {
        bs_search_state_free(state);
    }

    free(threads);
    free(ranges);    
    
    bs_context_stop(context, 0);
    context->callbacks.handle_search_complete(context->callbacks.userData);
    bs_context_release(context);
    
    return NULL;
}

static void * _bs_resume_dispatch(void * _context) {
    BSSearchContext * context = (BSSearchContext *)_context;
    BSSearchState * state = _bs_search_state_create(context);
    BSSearchState * lastState = context->saveData;
    int depth = lastState->depth;
    int threadCount = lastState->threadCount;
    assert(threadCount > 0);
    
    pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) * threadCount);
    
    state->progress = lastState->progress;
    state->depth = lastState->depth;
    int i;
    
    context->callbacks.handle_depth_increase(context->callbacks.userData, depth);
    
    for (i = 0; i < threadCount; i++) {
        BSThreadState * ts = lastState->states[i];
        BSThreadContext * ctx = _bs_thread_context_load(ts, depth, context);
        ctx->threadIndex = i;
        pthread_create(&threads[i], NULL, &_bs_search_thread, ctx);
    }
    for (i = 0; i < threadCount; i++) {
        void * savedData = NULL;
        pthread_join(threads[i], &savedData);
        if (savedData) {
            _bs_search_state_add_thread(state, (BSThreadState *)savedData);
        }
    }
    state->progress = context->progress;
    
    free(threads);
    
    // if the search wasn't cancelled, we might as well
    // return control over to the main search dispatch.
    if (!bs_context_is_stopped(context)) {
        bs_search_state_free(state);
        pthread_mutex_lock(&context->mutex);
        context->currentDepth++;
        pthread_mutex_unlock(&context->mutex);
        return _bs_run_dispatch(_context);
    }
    
    if (bs_context_should_save(context)) {
        context->callbacks.handle_save_data(context->callbacks.userData,
                                            state);
    } else bs_search_state_free(state);
    
    bs_context_stop(context, 0);
    context->callbacks.handle_search_complete(context->callbacks.userData);
    bs_context_release(context);
    
    return NULL;
    
}

static void * _bs_search_thread(void * threadContext) {
    BSThreadContext * context = (BSThreadContext *)threadContext;
    
    _bs_recursive_search(context);
    int shouldSave = bs_context_should_save(context->context);
    
    BSThreadState * saveState = NULL;
    if (shouldSave) {
        saveState = _bs_thread_context_save(context);
    }
    
    // make sure the BSProgress is completely accurate
    _bs_recursive_search_progress_update(context);
    
    _bs_thread_context_free(context);
    pthread_exit(saveState);
    return saveState;
}

static int _bs_recursive_search(BSThreadContext * context) {
    BSCallbacks callbacks = context->context->callbacks;
    if (context->currentDepth == context->depth) {
        context->nodeCount++;
        return _bs_recursive_search_hit_base(context);
    }
    if (!callbacks.should_expand(callbacks.userData,
                                 context->sequence, context->currentDepth,
                                 context->depth, context->threadIndex)) {
        context->pruneCount++;
        return 1;
    }
    context->nodeCount++;
    if (context->nodeCount > context->context->settings.nodeInterval ||
        context->lastUpdate < time(NULL)) {
        if (!_bs_recursive_search_progress_update(context)) return 0;
    }
    
    int min = srange_minimum_digit(context->range, context->currentDepth, context->sequence);
    int max = srange_maximum_digit(context->range, context->currentDepth, context->sequence);
    int i;
    for (i = min; i <= max; i++) {
        context->sequence[context->currentDepth] = i;
        context->currentDepth++;
        if (!_bs_recursive_search(context)) return 0;
        context->currentDepth--;
    }
    
    return 1;
}

static int _bs_recursive_search_hit_base(BSThreadContext * context) {
    BSCallbacks callbacks = context->context->callbacks;
    callbacks.handle_reached_node(callbacks.userData, context->sequence,
                                  context->currentDepth, context->threadIndex);
    return 1;
}

static int _bs_recursive_search_progress_update(BSThreadContext * _context) {
    BSSearchContext * context = _context->context;
    pthread_mutex_lock(&context->mutex);
    if (!context->isRunning) {
        pthread_mutex_unlock(&context->mutex);
        return 0;
    }
    context->progress.nodesExpanded += _context->nodeCount;
    context->progress.nodesPruned += _context->pruneCount;
    _context->nodeCount = 0;
    _context->pruneCount = 0;
    _context->lastUpdate = time(NULL);
    
    BSCallbacks callbacks = context->callbacks;
    callbacks.handle_progress_update(callbacks.userData);
    pthread_mutex_unlock(&context->mutex);
    return 1;
}
