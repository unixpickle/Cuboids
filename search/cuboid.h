#ifndef __SEARCH_CUBOID_H__
#define __SEARCH_CUBOID_H__

#include <time.h>

#include "notation/alg_list.h"

#include "sequence_cache.h"
#include "base.h"

typedef struct {
    uint8_t cacheCuboid;
    
    Cuboid * rootNode;
    AlgList * algorithms;
} CSSettings;

typedef struct {
    BSSearchState * bsState;
    CSSettings settings;
} CSSearchState;

typedef struct {
    void * userData;
    
    // Called when the internal base searcher makes a progress update
    void (*handle_progress)(void * data);
    
    // Called when the underlying search changes depth
    void (*handle_depth)(void * data, int depth);
    
    // Called to validate every sequence explored
    int (*accepts_sequence)(void * data, const int * sequence,
                            int len, int depthRemaining);
    
    // Called to validate a cuboid. The StickerMap argument
    // will be non-NULL unless cacheStickerMaps is set to 0.
    int (*accepts_cuboid)(void * data, const Cuboid * cuboid,
                          Cuboid * cache, int depthRemaining);
                          
    // Called for each root node which is found
    void (*handle_cuboid)(void * data, const Cuboid * cuboid, Cuboid * cache,
                          const int * sequence, int len);
                          
    // Called if the context was saved
    void (*handle_save_data)(void * data, CSSearchState * save);
                          
    // Called when the search is complete either because of a pause, stop
    // or a general exhaustion case.
    void (*handle_finished)(void * data);
} CSCallbacks;

typedef struct {
    CSSettings settings;
    CSCallbacks callbacks;
    
    int retainCount;
    pthread_mutex_t mutex;
    
    BSSearchContext * bsContext;
    uint8_t isStopping;
    time_t startTime;
    
    SequenceCache ** caches;
} CSSearchContext;

/**
 * Spawns a new Cuboid search.
 * @argument settings The search settings. The AlgList and Cuboid
 * which are passed to this transfer ownership to the context. Do
 * not access or free them after calling this function.
 * @argument callbacks The callbacks.
 * @return The search context which is returned must be released with
 * cs_context_release.
 */
CSSearchContext * cs_run(CSSettings settings, BSSettings bsSettings, CSCallbacks callbacks);

/**
 * Resumes a Cuboid search.
 * @argument state The saved search state. This state is owned by the
 * search context once it is passed. Do not free or access the state
 * after calling this function.
 * @argument callbacks The callbacks.
 * @return See cs_run return.
 */
CSSearchContext * cs_resume(CSSearchState * state, CSCallbacks callbacks);

void cs_context_release(CSSearchContext * context);
void cs_context_retain(CSSearchContext * context);
int cs_context_is_running(CSSearchContext * context);
void cs_context_stop(CSSearchContext * context, int save);

void cs_search_state_free(CSSearchState * state);

#endif
