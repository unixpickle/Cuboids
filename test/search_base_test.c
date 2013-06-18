#include "search/base.h"
#include "test.h"

typedef struct {
    BSSearchState * save;
} CbData;

static volatile unsigned long long nodeCount = 0;

void test_pause_resume();
void test_excessive_threads();

BSCallbacks generate_callbacks();

void cb_reached_node_counter(void * cbPtr, const int * seq, int depth, int th);
void cb_handle_depth_increase(void * data, int depth);
void cb_handle_search_complete(void * data);
int cb_should_expand(void * data, const int * seq, int len, int depth, int th);
void cb_handle_save_data(void * data, void * save);
void cb_handle_progress_update(void * data);


int main() {
    test_excessive_threads();
    test_pause_resume();    
    
    tests_completed();
    return 0;
}

void test_pause_resume() {
    test_initiated("pause/resume");
    
    BSSettings settings;
    settings.operationCount = 30;
    settings.threadCount = 8;
    settings.minDepth = 1;
    settings.maxDepth = 6;
    settings.nodeInterval = 1000000;
    
    CbData data;
    bzero(&data, sizeof(data));
    BSCallbacks callbacks = generate_callbacks();
    callbacks.userData = &data;
    
    nodeCount = 0;
    
    BSSearchContext * context = bs_run(settings, callbacks);
    while (!bs_context_is_stopped(context)) {
        unsigned long long val = __sync_or_and_fetch(&nodeCount, 0);
        if (val > 364500000L) {
            puts("Roughly half complete; pausing and resuming search...");
            bs_context_stop(context, 1);
            break;
        } else {
            sleep(1);
        }
    }
    
    bs_context_release(context);
    
    if (!data.save) {
        puts("Error: search returned no save data");
        printf("(expanded %lld nodes).\n", nodeCount);
        test_completed();
        return;
    }
    
    context = bs_resume(data.save, callbacks);
    while (!bs_context_is_stopped(context)) {
        sleep(1);
    }
    
    bs_context_release(context);
    if (nodeCount != 754137930L) {
        printf("Error: expected 15943877550 nodes but got %lld.\n", nodeCount);
    }
    
    test_completed();
}

void test_excessive_threads() {
    test_initiated("excessive threads");
    
    BSSettings settings;
    settings.operationCount = 20;
    settings.threadCount = 50;
    settings.minDepth = 1;
    settings.maxDepth = 6;
    settings.nodeInterval = 1000000;
    
    CbData data;
    bzero(&data, sizeof(data));
    BSCallbacks callbacks = generate_callbacks();
    callbacks.userData = &data;
    
    nodeCount = 0;
    
    BSSearchContext * context;
    context = bs_run(settings, callbacks);
    while (!bs_context_is_stopped(context)) {
        sleep(1);
    }
    
    if (nodeCount != 67368420L) {
        printf("Error: expected 15943877550 nodes but got %lld.\n", nodeCount);
    }
    
    bs_context_release(context);
    test_completed();
}


BSCallbacks generate_callbacks() {
    BSCallbacks callbacks;
    callbacks.handle_reached_node = cb_reached_node_counter;
    callbacks.handle_depth_increase = cb_handle_depth_increase;
    callbacks.handle_search_complete = cb_handle_search_complete;
    callbacks.should_expand = cb_should_expand;
    callbacks.handle_save_data = cb_handle_save_data;
    callbacks.handle_progress_update = cb_handle_progress_update;
    return callbacks;
}

void cb_reached_node_counter(void * cbPtr, const int * seq, int depth, int th) {
    unsigned long long count = __sync_add_and_fetch(&nodeCount, 1);
}

void cb_handle_depth_increase(void * data, int depth) {
    
}

void cb_handle_search_complete(void * data) {
    
}

int cb_should_expand(void * data, const int * seq, int len, int depth, int th) {
    return 1;
}

void cb_handle_save_data(void * _data, void * save) {
    CbData * data = (CbData *)_data;
    data->save = (BSSearchState *)save;
}

void cb_handle_progress_update(void * data) {
}
