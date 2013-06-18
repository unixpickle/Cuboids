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
    void (*handle_reached_node)(void * data, const int * sequence,
                                int depth, int threadIndex);
    
    // called to indicate a depth increase; the search thread waits on the return
    // of this function to begin its search.
    void (*handle_depth_increase)(void * data, int depth);
    
    // called to verify if a sequence should be expanded to achieve a certain death
    int (*should_expand)(void * data, const int * sequence, int len,
                         int depth, int threadIndex);
    
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

/**
 * Initializes a search in the background and returns a search context
 * @return The returned context must be released with bs_context_release().
 */
BSSearchContext * bs_run(BSSettings settings, BSCallbacks callbacks);

/**
 * Initializes a search in the background using a saved search state.
 * @argument state The state to restore from. Ownership of the state
 * is transfered to the search context, so the caller should no longer
 * access or free the state after calling bs_resume().
 * @return See bs_run return value.
 */
BSSearchContext * bs_resume(BSSearchState * state, BSCallbacks callbacks);

void bs_context_retain(BSSearchContext * context);
void bs_context_release(BSSearchContext * context);

/**
 * Sends a search context the message to halt. This function
 * will block until the search context has terminated all its
 * background threads.
 */
void bs_context_stop(BSSearchContext * context, int save);

/**
 * Returns the progress of a search context. This function is
 * completely synchronized, so don't worry about incorrect data.
 * or calling from multiple threads.
 */
BSProgress bs_context_progress(BSSearchContext * context);

/**
 * Returns the current depth of a search context. This increments
 * as the IDA* search deepens.
 */
int bs_context_current_depth(BSSearchContext * context);
int bs_context_is_stopped(BSSearchContext * context);
int bs_context_should_save(BSSearchContext * context);

void bs_search_state_free(BSSearchState * state);
