#include "data_list.h"

static int data_list_node_subnode_index(DataListNode * node, uint8_t subnodeByte, int * found);
static void data_list_node_add_subnode(DataListNode * node, DataListNode * subnode, int index);

static int data_list_base_entry_size(DataList * list);
static long long data_list_base_entry_index(DataListNode * base, const uint8_t * entry, int * found);
static int _compare_data(const uint8_t * left, const uint8_t * right, int len);

DataList * data_list_create(int dataSize, int headerLen, int shardDepth) {
    assert(dataSize >= shardDepth);
    DataList * list = (DataList *)malloc(sizeof(DataList));
    bzero(list, sizeof(DataList));
    list->dataSize = dataSize;
    list->headerLen = headerLen;
    list->depth = shardDepth;
    
    DataListNode * node = (DataListNode *)malloc(sizeof(DataListNode));
    bzero(node, sizeof(DataListNode));
    node->list = list;
    list->rootNode = node;
    
    return list;
}

void data_list_free(DataList * list) {
    data_list_node_free((DataListNode *)list->rootNode);
    free(list);
}

void data_list_node_free(DataListNode * node) {
    if (node->nodeData) {
        free(node->nodeData);
    }
    if (node->subnodes) {
        int i;
        for (i = 0; i < node->subnodeCount; i++) {
            DataListNode * theNode = (DataListNode *)node->subnodes[i];
            data_list_node_free(theNode);
        }
        free(node->subnodes);
    }
    free(node);
}

DataListNode * data_list_find_base(DataList * list, const uint8_t * body, int create) {
    DataListNode * current = (DataListNode *)list->rootNode;
    int depth;
    for (depth = 0; depth < list->depth; depth++) {
        int found;
        int index = data_list_node_subnode_index(current, body[depth], &found);
        if (found) {
            current = (DataListNode *)current->subnodes[index];
        } else if (create) {
            DataListNode * insertMe = (DataListNode *)malloc(sizeof(DataListNode));
            bzero(insertMe, sizeof(DataListNode));
            insertMe->list = list;
            insertMe->nodeByte = body[depth];
            insertMe->depth = depth + 1;
            data_list_node_add_subnode(current, insertMe, index);
            current = insertMe;
        } else return NULL;
    }
    return current;
}

int data_list_base_add(DataListNode * node, const uint8_t * body, const uint8_t * header) {
    int found;
    long long index = data_list_base_entry_index(node, body, &found);
    if (found) return 0;

    long long entrySize = data_list_base_entry_size(node->list);
    assert(entrySize > 0);

    if (node->dataSize + entrySize > node->dataAlloc) {
        unsigned long long newSize = node->dataAlloc;
        newSize += kBasenodeAllocBuffer * entrySize;
        if (!node->nodeData) {
            node->nodeData = (uint8_t *)malloc(newSize);
        } else {
            node->nodeData = (uint8_t *)realloc(node->nodeData, newSize);
        }
        node->dataAlloc = newSize;
    }

    long long offset = index * entrySize;
    long long moveSize = node->dataSize - offset;
    if (moveSize > 0) {
        uint8_t * source = &node->nodeData[offset];
        uint8_t * dest = &node->nodeData[offset + entrySize];
        memmove(dest, source, moveSize);
    }
    
    const uint8_t * bodyBuffer = &body[node->list->depth];
    long long bodyLen = node->list->dataSize - node->list->depth;
    memcpy(&node->nodeData[offset], header, node->list->headerLen);
    memcpy(&node->nodeData[offset + node->list->headerLen], bodyBuffer, bodyLen);
    
    node->dataSize += entrySize;
    
    return 1;
}

int data_list_base_find(DataListNode * node, const uint8_t * body, uint8_t ** headerOut) {
    int found;
    long long index = data_list_base_entry_index(node, body, &found);
    if (!found) return 0;
    long long dataSize = data_list_base_entry_size(node->list);
    uint8_t * ptr = &node->nodeData[dataSize * index];
    *headerOut = ptr;
    return 1;
}


/***********
 * Private *
 ***********/

static int data_list_node_subnode_index(DataListNode * node, uint8_t subnodeByte, int * found) {    
    int lowIndex = -1;
    int highIndex = node->subnodeCount;
    if (found) *found = 0;
    while (highIndex - lowIndex > 1) {
        int testIndex = (lowIndex + highIndex) / 2;
        DataListNode * subnode = (DataListNode *)node->subnodes[testIndex];
        if (subnode->nodeByte > subnodeByte) {
            highIndex = testIndex;
        } else if (subnode->nodeByte < subnodeByte) {
            lowIndex = testIndex;
        } else {
            highIndex = testIndex;
            lowIndex = testIndex;
            if (found) *found = 1;
        }
    }
    return highIndex;
}

static void data_list_node_add_subnode(DataListNode * node, DataListNode * subnode, int index) {
    if (node->subnodeAlloc == node->subnodeCount) {
        int newCount = node->subnodeAlloc + kSubnodeAllocBuffer;
        int newSize = sizeof(void *) * newCount;
        if (node->subnodeAlloc > 0) {
            node->subnodes = (void **)realloc(node->subnodes, newSize);
        } else {
            node->subnodes = (void **)malloc(newSize);
        }
        node->subnodeAlloc += kSubnodeAllocBuffer;
    }
    int moveCount = node->subnodeCount - index;
    if (moveCount > 0) {
        int moveSize = moveCount * sizeof(void *);
        void * moveStart = &node->subnodes[index];
        void * moveDest = &node->subnodes[index + 1];
        memmove(moveDest, moveStart, moveSize);
    }
    node->subnodes[index] = subnode;
    node->subnodeCount++;
}

/**************
 * Base nodes *
 **************/

static int data_list_base_entry_size(DataList * list) {
    return list->dataSize + list->headerLen - list->depth;
}

static long long data_list_base_entry_index(DataListNode * base, const uint8_t * entry, int * found) {
    assert(base->depth == base->list->depth);
    
    const uint8_t * baseBuffer = &entry[base->depth];
    long long bodySize = base->list->dataSize - base->depth;
    long long entrySize = data_list_base_entry_size(base->list);
    assert(entrySize > 0);
    
    if (found) *found = 0;
    
    long long lowIndex = -1;
    long long highIndex = base->dataSize / entrySize;
    while (highIndex - lowIndex > 1) {
        long long testIndex = (lowIndex + highIndex) / 2;
        uint8_t * buffer = &base->nodeData[testIndex * entrySize];
        uint8_t * entryBody = &buffer[base->list->headerLen];
        int comparison = _compare_data(entryBody, baseBuffer, bodySize);
        if (comparison > 0) {
            highIndex = testIndex;
        } else if (comparison < 0) {
            lowIndex = testIndex;
        } else {
            highIndex = testIndex;
            lowIndex = testIndex;
            if (found) *found = 1;
        }
    }
    return highIndex;
}

static int _compare_data(const uint8_t * left, const uint8_t * right, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if (left[i] < right[i]) return -1;
        if (left[i] > right[i]) return 1;
    }
    return 0;
}
