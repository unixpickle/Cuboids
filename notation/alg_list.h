#ifndef __ALG_LIST_H__
#define __ALG_LIST_H__

#include "parser.h"
#include "cuboid.h"

typedef struct {
    Algorithm * algorithm;
    Cuboid * cuboid;
} AlgListEntry;

typedef struct {
    int retainCount;
    
    AlgListEntry * entries;
    int entryCount;
} AlgList;

AlgList * alg_list_create();
void alg_list_add(AlgList * list, AlgListEntry entry);
void alg_list_release(AlgList * list);
void alg_list_retain(AlgList * list);

AlgList * alg_list_parse(const char * buffer, CuboidDimensions dims);

#endif
