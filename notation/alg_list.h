#include "parser.h"
#include "cuboid.h"

typedef struct {
    Algorithm * algorithm;
    Cuboid * cuboid;
} AlgListEntry;

typedef struct {
    AlgListEntry * entries;
    int entryCount;
} AlgList;

AlgList * alg_list_create();
void alg_list_add(AlgList * list, AlgListEntry entry);
void alg_list_free(AlgList * list);

AlgList * alg_list_parse(const char * buffer, CuboidDimensions dims);
