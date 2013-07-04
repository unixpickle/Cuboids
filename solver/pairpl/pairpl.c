#include "pairpl.h"

static int _pair_are_centers_solved(const Cuboid * cb);

typedef struct {
    uint8_t solveCenters;
} PairPLContext;

CLArgumentList * pairpl_default_arguments() {
    CLArgumentList * list = cl_argument_list_new();
    
    cl_argument_list_add(list, cl_argument_new_flag("centers", 0));
    
    return list;
}

int pairpl_run(CLSearchParameters * params, CLArgumentList * list, void ** userData) {
    PairPLContext * context = (PairPLContext *)malloc(sizeof(PairPLContext));
    
    // other options
    int idx = cl_argument_list_find(list, "centers");
    CLArgument * argument = cl_argument_list_get(list, idx);
    context->solveCenters = argument->contents.flag.boolValue;
    
    *userData = context;
    return 1;
}

int pairpl_resume(CLSearchParameters * params, FILE * fp, void ** userData) {
    uint8_t centerFlag = 0;
    if (fread(&centerFlag, 1, 1, fp) != 1) return 0;
    if (centerFlag > 1) return 0; 
    
    PairPLContext * context = (PairPLContext *)malloc(sizeof(PairPLContext));
    context->solveCenters = centerFlag;
    *userData = context;
    
    return 1;
}

void pairpl_save(void * data, FILE * fp) {
    PairPLContext * context = (PairPLContext *)data;
    uint8_t solveCenters = context->solveCenters;
    fwrite(&solveCenters, 1, 1, fp);
}

void pairpl_completed(void * data) {
    free(data);
}

int pairpl_is_goal(void * data, const Cuboid * cb, Cuboid * cache) {
    PairPLContext * context = (PairPLContext *)data;
    int i;
    for (i = 0; i < 12; i++) {
        int solid = cuboid_is_dedge_paired(cb, i);
        if (!solid) return 0;
    }
    if (context->solveCenters) {
        if (!_pair_are_centers_solved(cb)) {
            return 0;
        }
    }
    return 1;
}

static int _pair_are_centers_solved(const Cuboid * cb) {
    int face, i;
    for (face = 1; face <= 6; face++) {
        int count = cuboid_count_centers_for_face(cb, face);
        for (i = 0; i < count; i++) {
            int index = cuboid_center_index(cb, face, i);
            CuboidCenter c = cb->centers[index];
            if (c.side != face) return 0;
        }
    }
    return 1;
}
