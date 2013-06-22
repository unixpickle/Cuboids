#include "algebra/rotation_cosets.h"
#include "arguments/search_args.h"
#include <stdio.h>

typedef struct {
    RotationCosets * cosets;
    int axis;
    int solveCenters;
} EOPluginContext;

CLArgumentList * eopl_default_arguments();
int eopl_run(CLSearchParameters * params, CLArgumentList * list, void ** userData);
int eopl_resume(CLSearchParameters * params, FILE * fp, void ** userData);
void eopl_save(void * data, FILE * fp);
void eopl_completed(void * data);
int eopl_is_goal(void * data, const Cuboid * cb, Cuboid * cache);
