#include "standardpl.h"

CLArgumentList * standardpl_default_arguments() {
    return cl_argument_list_new();
}

int standardpl_run(CLArgumentList * list, void ** userData) {
    if (list->count > 0) {
        return 0;
    }
    *userData = NULL;
    return 1;
}

int standardpl_resume(FILE * fp, void ** userData) {
    *userData = NULL;
    return 1;
}

void standardpl_save(void * data, FILE * fp) {
    // nothing particular to save
}

void standardpl_completed(void * data) {
    // no resources to be freed!
}

void standardpl_is_goal(void * data, Cuboid * cb, StickerMap * cache) {
    
}