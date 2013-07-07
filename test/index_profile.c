#include "heuristic/heuristic_io.h"
#include "test.h"

void find_move_counts(DataList * list);
void recursive_count(DataListNode * node, uint64_t * counts);

int main(int argc, const char * argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <index file> <x> <y> <z>\n", argv[0]);
        return 1;
    }
    
    CuboidDimensions dims;
    dims.x = atoi(argv[2]);
    dims.y = atoi(argv[3]);
    dims.z = atoi(argv[4]);
    Heuristic * h = heuristic_from_file(argv[1], dims);
    
    if (!h) {
        puts("Error: failed to load index.");
        return 1;
    }
    
    printf("%d data symmetries, %d data cosets, %d moveset cosets\n", 
        rotation_group_count(h->dataSymmetries),
        rotation_cosets_count(h->dataCosets),
        h->cosetCount);
    
    int i;
    for (i = 0; i < h->cosetCount; i++) {
        printf("Distribution for coset %d:\n", i);
        find_move_counts(h->cosets[i]);
    }
    
    heuristic_free(h);
    
    return 0;
}

void find_move_counts(DataList * list) {
    uint64_t counts[256];
    bzero(counts, sizeof(uint64_t) * 256);
    recursive_count((DataListNode *)list->rootNode, counts);
    int i;
    for (i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            printf("%d - %llu\n", i, (unsigned long long)counts[i]);
        }
    }
}

void recursive_count(DataListNode * node, uint64_t * counts) {
    if (node->dataSize > 0) {
        long long entrySize = node->list->dataSize - node->depth + node->list->headerLen;
        long long i, count = node->dataSize / entrySize;
        for (i = 0; i < count; i++) {
            uint8_t * headerData = &node->nodeData[entrySize * i];
            counts[headerData[0]]++;
        }
    }
    int i;
    for (i = 0; i < node->subnodeCount; i++) {
        DataListNode * subnode = (DataListNode *)node->subnodes[i];
        recursive_count(subnode, counts);
    }
}
