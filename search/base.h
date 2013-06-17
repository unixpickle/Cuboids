/*
 * This is the base search agent.
 * 
 * The main purpose for this search agent is to implement a
 * general uniform cost iterative depth first search.
 *
 * The search includes multithreading and resuming out of the box.
 */

#include <time.h>
#include <pthread.h>
#include "boundary.h"

/************
 * Progress *
 ************/

typedef struct {
    unsigned long long nodesExpanded;
    unsigned long long nodesPruned;
} BSProgress;

/*******************
 * User structures *
 *******************/

typedef struct {
    void * userData;
    
    // called whenever the search reaches a node of the current depth
    void (*handle_reached_node)(void * data, const int * sequence, int depth);
    
    // called to indicate a depth increase
    void (*handle_depth_increase)(void * data, int depth);
    
    // called to verify if a sequence should be expanded to achieve a certain death
    int (*should_expand)(void * data, const int * sequence, int len, int depth);
    
    // called to give the delegate save data
    void (*handle_save_data)(void * data, void * save); // save will be a BSSearchState
    
    // called to give the delegate a progress update
    void (*handle_progress_update)(void * data);
    
    // called if the search completes
    void (*handle_search_complete)(void * data);
} BSCallbacks;

typedef struct {
    int operationCount;
    int threadCount;
    
    int minDepth;
    int maxDepth;
    
    int nodeInterval;
} BSSettings;

/***************
 * Save states *
 ***************/

typedef struct {
    SRange range;
} BSThreadState;

typedef struct {
    int depth;
    BSThreadState ** states;
    int threadCount;
    
    BSSettings settings;
    BSProgress progress;
} BSSearchState;

/********************
 * A search context *
 ********************/

typedef struct {
    BSSettings settings;
    BSCallbacks callbacks;
    int retainCount;
    
    pthread_mutex_t mutex;
    int isRunning;
    int shouldSave;
    BSProgress progress;
    int currentDepth;
    
    pthread_t dispatchThread;
    
    // this will only be non-NULL if the session
    // was resumed from a saved state.
    BSSearchState * saveData;
} BSSearchContext;

BSSearchContext * bs_run(BSSettings settings, BSCallbacks callbacks);
BSSearchContext * bs_resume(BSSearchState * state, BSCallbacks callbacks);

void bs_context_retain(BSSearchContext * context);
void bs_context_release(BSSearchContext * context);
void bs_context_stop(BSSearchContext * context, int save);
BSProgress bs_context_progress(BSSearchContext * context);
int bs_context_current_depth(BSSearchContext * context);
int bs_context_is_stopped(BSSearchContext * context);
int bs_context_should_save(BSSearchContext * context);

void bs_search_state_free(BSSearchState * state);
