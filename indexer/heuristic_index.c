#include "heuristic_index.h"

#define kNodeDepth 3

static HSParameters _process_heuristic_parameters(IndexerArguments args);
static int _data_list_append(DataList * dl, const uint8_t * data, const uint8_t * header);

HeuristicIndex * heuristic_index_create(CLArgumentList * args, IndexerArguments indexArgs,
                                        const char * name) {
    HSParameters params = _process_heuristic_parameters(indexArgs);
    Heuristic * heuristic = heuristic_create(params, args, name);
    if (!heuristic) return 0;
    
    int dataSize = heuristic_data_size(heuristic);
    int shardDepth = indexArgs.shardDepth;
    int nodeDepth = dataSize < shardDepth ? dataSize : shardDepth;
    
    // generate cosets
    RotationBasis basis = rotation_basis_standard(heuristic->params.symmetries.dims);
    RotationGroup * subgroup = rotation_group_create_basis(heuristic->params.symmetries);
    RotationGroup * group = rotation_group_create_basis(basis);
    RotationCosets * cosets = rotation_cosets_create(group, subgroup);
    
    int cosetCount = rotation_cosets_count(cosets);
    Cuboid ** inverseTriggers = (Cuboid **)malloc(sizeof(void *) * cosetCount);
    
    int i;
    for (i = 0; i < cosetCount; i++) {
        DataList * dl = data_list_create(dataSize, 2, nodeDepth);
        heuristic_add_coset(heuristic, dl);
        Cuboid * cuboid = rotation_cosets_get_trigger(cosets, i);
        Cuboid * inv = cuboid_inverse(cuboid);
        inverseTriggers[i] = inv;
    }
    
    rotation_group_release(group);
    rotation_cosets_release(cosets);
    rotation_group_release(subgroup);
    
    HeuristicIndex * index = (HeuristicIndex *)malloc(sizeof(HeuristicIndex));
    index->heuristic = heuristic;
    index->invTriggers = inverseTriggers;
    return index;
}

void heuristic_index_free(HeuristicIndex * index) {
    int i, cosetCount = index->heuristic->cosetCount;
    for (i = 0; i < cosetCount; i++) {
        cuboid_free(index->invTriggers[i]);
    }
    free(index->invTriggers);
    heuristic_free(index->heuristic);
    free(index);
}

int heuristic_index_accepts_node(HeuristicIndex * index, int depth, int idaDepth,
                                 const Cuboid * cb, Cuboid * cache) {
    // we must check all the angles to see if we have
    // found a shorter path to a Cuboid than previously.
    cuboid_multiply(cache, cb, index->invTriggers[0]);
    uint8_t * indexData = (uint8_t *)malloc(heuristic_data_size(index->heuristic));
    Cuboid * temp = cuboid_copy(cb);
    int i, numAngles = index->heuristic->angles->numDistinct;
    int accepts = 0;
    for (i = 0; i < numAngles; i++) {
        int angle = index->heuristic->angles->distinct[i];
        heuristic_get_data(index->heuristic, cache, temp, angle, indexData);
        DataList * dataList = index->heuristic->cosets[0];
        DataListNode * base = data_list_find_base(dataList, indexData, 0);
        if (!base) {
            accepts = 1;
            continue;
        }
        
        uint8_t * header;
        int found = data_list_base_find(base, indexData, &header);
        if (!found) {
            accepts = 1;
            continue;
        }
    
        int value = header[0];
        if (value < depth) {
            continue;
        }
        if (value == depth && idaDepth == header[1]) {
            continue;
        }
        
        // if we found it at a new depth, we should set that here
        header[1] = idaDepth;
        accepts = 1;
    }
    free(indexData);
    cuboid_free(temp);
    return accepts;
}

int heuristic_index_add_node(HeuristicIndex * index, const Cuboid * cb, Cuboid * cache, int depth) {
    uint8_t headerData[2] = {depth, depth};
    
    uint8_t * data = (uint8_t *)malloc(heuristic_data_size(index->heuristic));
    int i, j, cosetCount = index->heuristic->cosetCount;
    int addedSomething = 0;
    Cuboid * temp = cuboid_copy(cb);
    
    for (i = 0; i < cosetCount; i++) {
        DataList * coset = index->heuristic->cosets[i];
        Cuboid * rot = index->invTriggers[i];
        cuboid_multiply(cache, cb, rot); // cache now contains our coset cube
        for (j = 0; j < index->heuristic->angles->numDistinct; j++) {
            int angle = index->heuristic->angles->distinct[j];
            heuristic_get_data(index->heuristic, cache, temp, angle, data);
            if (_data_list_append(coset, data, headerData)) {
                addedSomething = 1;
            }
        }
    }
    free(data);
    cuboid_free(temp);
    return addedSomething;
}

/***********
 * Private *
 ***********/

static HSParameters _process_heuristic_parameters(IndexerArguments args) {
    HSParameters params;
    params.symmetries = args.symmetries;
    params.maxDepth = args.maxDepth;
    return params;
}

static int _data_list_append(DataList * dl, const uint8_t * data, const uint8_t * header) {
    DataListNode * base = data_list_find_base(dl, data, 1);
    return data_list_base_add(base, data, header);
}
