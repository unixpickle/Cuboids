#include "algebra/idcache.h"
#include "arguments/search_args.h"
#include "stickers/stickermap.h"
#include <stdio.h>

CLArgumentList * standardpl_default_arguments();
int standardpl_run(CLSearchParameters * params, CLArgumentList * list, void ** userData);
int standardpl_resume(CLSearchParameters * params, FILE * fp, void ** userData);
void standardpl_save(void * data, FILE * fp);
void standardpl_completed(void * data);
int standardpl_is_goal(void * data, Cuboid * cb, StickerMap * cache);
