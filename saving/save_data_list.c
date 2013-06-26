#include "save_data_list.h"

static void _save_data_list_node(DataListNode * node, FILE * fp);
static DataListNode * _load_data_list_node(DataList * parent, FILE * fp);
static DataListNode ** _load_subnodes(DataList * parent, int count, FILE * fp);
static int _load_data(uint64_t * lenOut, uint8_t ** dataOut, FILE * fp);

void save_data_list(DataList * list, FILE * fp) {
    save_uint32(list->dataSize, fp);
    save_uint32(list->headerLen, fp);
    save_uint32(list->depth, fp);
    _save_data_list_node((DataListNode *)list->rootNode, fp);
}

DataList * load_data_list(FILE * fp) {
    uint32_t dataSize, headerLen, depth;
    if (!load_uint32(&dataSize, fp)) return NULL;
    if (!load_uint32(&headerLen, fp)) return NULL;
    if (!load_uint32(&depth, fp)) return NULL;
    DataList * list = (DataList *)malloc(sizeof(DataList));
    list->dataSize = dataSize;
    list->headerLen = headerLen;
    list->depth = depth;
    DataListNode * root = _load_data_list_node(list, fp);
    if (!root) {
        free(list);
        return NULL;
    }
    list->rootNode = root;
    return list;
}

/***********
 * Private *
 ***********/

static void _save_data_list_node(DataListNode * node, FILE * fp) {
    int i;
    
    save_uint8(node->nodeByte, fp);
    save_uint8(node->depth, fp);
    
    save_uint16(node->subnodeCount, fp);
    for (i = 0; i < node->subnodeCount; i++) {
        DataListNode * subnode = (DataListNode *)node->subnodes[i];
        _save_data_list_node(subnode, fp);
    }
    
    save_uint64(node->dataSize, fp);
    if (node->dataSize > 0) {
        fwrite(node->nodeData, 1, node->dataSize, fp);
    }
}

static DataListNode * _load_data_list_node(DataList * parent, FILE * fp) {
    int i;
    uint8_t nodeByte, depth;
    uint16_t subnodeCount;
    uint64_t dataSize;

    if (!load_uint8(&nodeByte, fp)) return NULL;
    if (!load_uint8(&depth, fp)) return NULL;
    if (!load_uint16(&subnodeCount, fp)) return NULL;
        
    DataListNode ** subnodes = _load_subnodes(parent, subnodeCount, fp);
    
    uint8_t * data;
    if (!_load_data(&dataSize, &data, fp)) {
        for (i = 0; i < subnodeCount; i++) {
            data_list_node_free(subnodes[i]);
        }
        if (subnodes) free(subnodes);
        return NULL;
    }
    
    DataListNode * node = (DataListNode *)malloc(sizeof(DataListNode));
    node->list = parent;
    node->subnodes = (void **)subnodes;
    node->subnodeCount = subnodeCount;
    node->subnodeAlloc = subnodeCount;
    node->nodeByte = nodeByte;
    node->depth = depth;
    node->nodeData = data;
    node->dataSize = dataSize;
    node->dataAlloc = dataSize;
    node->list = parent;
    return node;
}

static DataListNode ** _load_subnodes(DataList * parent, int count, FILE * fp) {
    int i, j;
    DataListNode ** subnodes = NULL;
    if (count > 0) {
        subnodes = (DataListNode **)malloc(sizeof(void *) * count);
        for (i = 0; i < count; i++) {
            DataListNode * subnode = _load_data_list_node(parent, fp);
            if (!subnode) {
                for (j = 0; j < i; j++) {
                    data_list_node_free(subnodes[j]);
                }
                free(subnodes);
                return NULL;
            }
            subnodes[i] = subnode;
        }
    }
    return subnodes;
}

static int _load_data(uint64_t * lenOut, uint8_t ** dataOut, FILE * fp) {
    if (!load_uint64(lenOut, fp)) {
        return 0;
    }
    uint8_t * data = NULL;
    if (lenOut[0] > 0) {
        data = (uint8_t *)malloc(lenOut[0]);
        if (fread(data, 1, lenOut[0], fp) != lenOut[0]) {
            free(data);
            return 0;
        }
    }
    *dataOut = data;
    return 1;
}
