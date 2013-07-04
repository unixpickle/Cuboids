#include "algebra/sticker_algebra.h"
#include "arguments/search_args.h"
#include <stdio.h>

CLArgumentList * pairpl_default_arguments();
int pairpl_run(CLSearchParameters * params, CLArgumentList * list, void ** userData);
int pairpl_resume(CLSearchParameters * params, FILE * fp, void ** userData);
void pairpl_save(void * data, FILE * fp);
void pairpl_completed(void * data);
int pairpl_is_goal(void * data, const Cuboid * cb, Cuboid * cache);
