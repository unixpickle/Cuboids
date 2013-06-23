#ifndef __DATA_LIST_H__
#define __DATA_LIST_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define kSubnodeAllocBuffer 4
#define kBasenodeAllocBuffer 512

/***
 * 
 * The data list allows for fast, dynamic searching and insertion of
 * binary data objects.
 *
 * Sharding is accomplished by taking the first `depth` bytes of data
 * and, instead of storing them in a giant buffer, storing them one byte
 * at a time in deeper and deeper nodes which are easy to traverse.
 *
 */

typedef struct {
    void * rootNode;
    int dataSize;
    int headerLen;
    int depth;
} DataList;

typedef struct {
    DataList * list;
    void ** subnodes;
    uint16_t subnodeCount;
    uint16_t subnodeAlloc;
    
    // information about every non-root node
    uint8_t nodeByte;
    uint8_t depth; // for root node
    
    // base node only
    uint8_t * nodeData;
    unsigned long long dataSize;
    unsigned long long dataAlloc;
} DataListNode;

/**
 * Creates a new data list.
 * @argument shardDepth  The maximum depth for a node. If this is equal to
 * dataSize, then no buffers will even need to be allocated!
 */
DataList * data_list_create(int dataSize, int headerLen, int shardDepth);

/**
 * Frees a DataList and all of its subnodes.
 */
void data_list_free(DataList * list);

/**
 * Frees a single data list node.
 */
void data_list_node_free(DataListNode * node);

/**
 * Finds the "base node" (that is, node of depth `list->depth`) which
 * would correspond to the body `body`.
 * @argument create If this is 1, the base node and all parents will be
 * created as necessary.
 */
DataListNode * data_list_find_base(DataList * list, const uint8_t * body, int create);

/**
 * Adds the body excluding the length indicated by `list->depth` to the node.
 * @return 1 if added, 0 if it was already in the list.
 */
int data_list_base_add(DataListNode * node, const uint8_t * body, const uint8_t * header);

/**
 * Finds the body excluding the length indicated by `list->depth` in the node.
 * @return 1 if found, 0 if not.
 */
int data_list_base_find(DataListNode * node, const uint8_t * body, uint8_t ** headerOut);

#endif