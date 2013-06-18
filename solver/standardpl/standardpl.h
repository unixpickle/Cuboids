#include "arguments/arguments.h"
#include "cuboid/cuboid.h"
#include "algebra/idcache.h"

CLArgumentList * standardpl_default_arguments();
int standardpl_run(CLArgumentList * list, void ** userData);
int standardpl_resume(FILE * fp, void ** userData);
void standardpl_save(void * data, FILE * fp);
void standardpl_completed(void * data);
void standardpl_is_goal(void * data, Cuboid * cb, StickerMap * cache);
