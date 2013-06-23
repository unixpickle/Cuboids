#include "saving/save_cuboid.h"
#include "saving/save_algorithm.h"
#include "saving/save_search.h"
#include "saving/save_data_list.h"
#include "notation/cuboid.h"
#include "notation/parser.h"
#include "test.h"

void test_save_cuboid();
void test_save_algorithm();
void test_save_base_search();
void test_save_cuboid_search();
void test_save_data_list();

BSSearchState * generate_bs_search_state();
DataList * generate_data_list();
void test_base_state_equality(BSSearchState * s1, BSSearchState * s2);
void test_cuboid_state_equality(CSSearchState * s1, CSSearchState * s2);
void test_srange_equality(SRange r1, SRange r2);
void test_cuboid_equality(Cuboid * c1, Cuboid * c2);
int test_algorithm_equality(Algorithm * a1, Algorithm * a2);
int test_data_list_node_equality(DataListNode * n1, DataListNode * n2);

int main() {
    test_save_cuboid();
    test_save_algorithm();
    test_save_base_search();
    test_save_cuboid_search();
    test_save_data_list();
    
    tests_completed();
    return 0;
}

void test_save_cuboid() {
    test_initiated("save_cuboid");
    
    CuboidDimensions dims = {10, 20, 10};
    Algorithm * algo = algorithm_for_string("R2 2Bw2 3Dw L2 5Bw2 F2 4Rw2 L2 Fw2 2Uw'");
    Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    
    FILE * temp = tmpfile();
    assert(temp != NULL);
    save_cuboid(cuboid, temp);
    fseek(temp, 0, SEEK_SET);
    Cuboid * cb = load_cuboid(temp);
    fclose(temp);
    
    if (!cb) {
        puts("Error: failed to load cuboid at all!");
    }
    
    test_cuboid_equality(cb, cuboid);
    
    cuboid_free(cb);
    cuboid_free(cuboid);
    test_completed();
}

void test_save_algorithm() {
    test_initiated("save_algorithm");
    
    Algorithm * algo = algorithm_for_string("R Uw 2Fw3 D' M2 x L2 y' z2");
    
    FILE * temp = tmpfile();
    assert(temp != NULL);
    
    save_algorithm(algo, temp);
    fseek(temp, 0, SEEK_SET);
    Algorithm * loaded = load_algorithm(temp);
    
    if (!loaded) {
        puts("Error: failed to load algorithm at all!");
    }
    if (loaded->type != AlgorithmTypeContainer) {
        puts("Error: invalid type for loaded algorithm.");
    }
    
    test_algorithm_equality(algo, loaded);
    
    fclose(temp);
    
    algorithm_free(algo);
    algorithm_free(loaded);
    test_completed();
}

void test_save_base_search() {
    test_initiated("save_base_search");
    
    BSSearchState * state = generate_bs_search_state();
    FILE * temp = tmpfile();
    assert(temp != NULL);
    
    save_base_search(state, temp);
    fseek(temp, 0, SEEK_SET);
    BSSearchState * loaded = load_base_search(temp);
    
    if (loaded) {
        test_base_state_equality(state, loaded);
        bs_search_state_free(loaded);
    } else {
        puts("Error: failed to load!");
    }
    
    fclose(temp);
    bs_search_state_free(state);
    test_completed();
}

void test_save_cuboid_search() {
    test_initiated("save_cuboid_search");
    
    FILE * temp = tmpfile();
    assert(temp != NULL);
    BSSearchState * bsState = generate_bs_search_state();
    CSSearchState * state = (CSSearchState *)malloc(sizeof(CSSearchState));
    state->bsState = bsState;
    
    CuboidDimensions dims = {5, 6, 6};
    Algorithm * testAlgo = algorithm_for_string("Lw R2 U2 3Dw2 L'");
    state->settings.rootNode = algorithm_to_cuboid(testAlgo, dims);
    algorithm_free(testAlgo);
    state->settings.algorithms = alg_list_parse("R,L,Uw2,Dw2,Fw2,Bw2", dims);
    
    save_cuboid_search(state, temp);
    fseek(temp, 0, SEEK_SET);
    CSSearchState * loaded = load_cuboid_search(temp);
    if (loaded) {
        test_cuboid_state_equality(loaded, state);
        cs_search_state_free(loaded);
    } else {
        puts("Error: failed to load search state.");
    }
    
    cs_search_state_free(state);
    fclose(temp);
    test_completed();
}

void test_save_data_list() {
    test_initiated("save_data_list");
    
    DataList * list = generate_data_list();
    FILE * temp = tmpfile();
    assert(temp != NULL);
    save_data_list(list, temp);
    fseek(temp, 0, SEEK_SET);
    DataList * loaded = load_data_list(temp);
    fclose(temp);
    
    if (loaded) {
        test_data_list_node_equality(loaded->rootNode, list->rootNode);
        if (loaded->dataSize != list->dataSize) {
            puts("Error: dataSize disagrees");
        }
        if (loaded->headerLen != list->headerLen) {
            puts("Error: headerLen disagrees");
        }
        if (loaded->depth != list->depth) {
            puts("Error: depth disagrees");
        }
    } else {
        puts("Error: failed to load data list.");
    }
    
    data_list_free(loaded);
    data_list_free(list);
    
    test_completed();
}

BSSearchState * generate_bs_search_state() {
    BSSearchState * state = (BSSearchState *)malloc(sizeof(BSSearchState));
    SBoundary b1, b2, b3, b4;
    sboundary_initialize(&b1, 5, 6);
    sboundary_initialize(&b2, 5, 6);
    sboundary_initialize(&b3, 5, 6);
    sboundary_initialize(&b4, 5, 6);
    
    int data1[5] = {0, 0, 0, 0};
    int data2[5] = {3, 0, 0, 1};
    int data3[5] = {3, 0, 0, 1};
    int data4[5] = {6, 0, 0, 0};
    memcpy(b1.sequence, data1, sizeof(int) * 5);
    memcpy(b2.sequence, data2, sizeof(int) * 5);
    memcpy(b3.sequence, data3, sizeof(int) * 5);
    memcpy(b4.sequence, data4, sizeof(int) * 5);
    
    BSThreadState * th1 = (BSThreadState *)malloc(sizeof(BSThreadState));
    BSThreadState * th2 = (BSThreadState *)malloc(sizeof(BSThreadState));
    
    th1->range.lower = b1;
    th1->range.upper = b2;
    th2->range.lower = b3;
    th2->range.upper = b4;
    
    state->threadCount = 2;
    state->states = (BSThreadState **)malloc(sizeof(void *) * 2);
    state->states[0] = th1;
    state->states[1] = th2;
    
    state->depth = 13;
    
    BSSettings dummySettings;
    BSProgress dummyProgress;
    
    dummySettings.operationCount = 18;
    dummySettings.threadCount = 10;
    dummySettings.minDepth = 2;
    dummySettings.maxDepth = 10;
    dummySettings.nodeInterval = 1000000;
    
    dummyProgress.nodesExpanded = 1000000L;
    dummyProgress.nodesPruned = 1000000000L;
    state->settings = dummySettings;
    state->progress = dummyProgress;
    
    return state;
}

DataList * generate_data_list() {
    DataList * list = data_list_create(4, 1, 2);
    uint8_t counters[4] = {0, 0, 0, 0};
    int isDone = 0;
    while (!isDone) {
        int i;
        uint8_t header = 0x12;
        for (i = 0; i < 4; i++) {
            header ^= counters[i];
        }
        DataListNode * node = data_list_find_base(list, counters, 1);
        assert(node != NULL);
        data_list_base_add(node, counters, &header);
        
        // increment counters
        for (i = 0; i < 4; i++) {
            if (counters[i] == 0x20) {
                counters[i] = 0;
                if (i == 3) isDone = 1;
            } else {
                counters[i]++;
                break;
            }
        }
    }
    return list;
}

void test_base_state_equality(BSSearchState * s1, BSSearchState * s2) {
    if (s1->threadCount != s2->threadCount) {
        puts("Error: thread counts differ.");
        return;
    }
    int i, j;
    for (i = 0; i < s1->threadCount; i++) {
        BSThreadState * th1 = s1->states[i];
        BSThreadState * th2 = s2->states[i];
        test_srange_equality(th1->range, th2->range);
    }
    if (s1->depth != s2->depth) {
        puts("Error: depths differ.");
    }
    if (s1->settings.minDepth != s2->settings.minDepth) {
        puts("Error: minDepth differs.");
    }
    if (s1->settings.maxDepth != s2->settings.maxDepth) {
        puts("Error: maxDepth differs.");
    }
    if (s1->settings.operationCount != s2->settings.operationCount) {
        puts("Error: operationCount differs.");
    }
    if (s1->settings.nodeInterval != s2->settings.nodeInterval) {
        puts("Error: nodeInterval differs.");
    }
    if (s1->settings.threadCount != s2->settings.threadCount) {
        puts("Error: settings.threadCount differs.");
    }
    if (s1->progress.nodesExpanded != s2->progress.nodesExpanded) {
        puts("Error: nodesExpanded differs.");
    }
    if (s1->progress.nodesPruned != s2->progress.nodesPruned) {
        puts("Error: nodesExpanded differs.");
    }
}

void test_cuboid_state_equality(CSSearchState * s1, CSSearchState * s2) {
    test_base_state_equality(s1->bsState, s2->bsState);
    test_cuboid_equality(s1->settings.rootNode, s2->settings.rootNode);
    if (s1->settings.algorithms->entryCount != s2->settings.algorithms->entryCount) {
        puts("Error: algorithm counts don't match.");
        return;
    }
    int i;
    for (i = 0; i < s1->settings.algorithms->entryCount; i++) {
        AlgListEntry e1 = s1->settings.algorithms->entries[i];
        AlgListEntry e2 = s2->settings.algorithms->entries[i];
        test_cuboid_equality(e1.cuboid, e2.cuboid);
        test_algorithm_equality(e1.algorithm, e2.algorithm);
    }
}

void test_srange_equality(SRange r1, SRange r2) {
    if (r1.lower.length != r2.lower.length || r1.upper.length != r2.upper.length) {
        puts("Error: range counts don't match.");
        return;
    }
    if (r1.upper.length != r1.lower.length) {
        puts("Error: upper and lower counts much match.");
        return;
    }
    int i;
    for (i = 0; i < r1.lower.length; i++) {
        if (r1.lower.sequence[i] != r2.lower.sequence[i]) {
            printf("Error: range lower differs at index %d.\n", i);
        }
        if (r1.upper.sequence[i] != r2.upper.sequence[i]) {
            printf("Error: range upper differs at index %d.\n", i);
        }
    }
}

void test_cuboid_equality(Cuboid * cuboid, Cuboid * cb) {
    if (!cuboid_dimensions_equal(cuboid->dimensions, cb->dimensions)) {
        puts("Error: cuboid dimensions are not equal");
        return;
    }
    
    int i, j;
    for (i = 0; i < 12; i++) {
        int edgeCount = cuboid_count_edges_for_dedge(cuboid, i);
        for (j = 0; j < edgeCount; j++) {
            int index = cuboid_edge_index(cuboid, i, j);
            CuboidEdge e1 = cuboid->edges[index];
            CuboidEdge e2 = cb->edges[index];
            if (e1.symmetry != e2.symmetry || e1.dedgeIndex != e2.dedgeIndex
                || e1.edgeIndex != e2.edgeIndex) {
                    printf("Error: edge mismatch at (%d, %d).\n", i, j);
            }
        }
    }
    for (i = 1; i <= 6; i++) {
        int count = cuboid_count_centers_for_face(cuboid, i);
        for (j = 0; j < count; j++) {
            int index = cuboid_center_index(cuboid, i, j);
            CuboidCenter c1 = cuboid->centers[index];
            CuboidCenter c2 = cb->centers[index];
            if (c1.side != c2.side || c1.index != c2.index) {
                printf("Error: center mismatch at (%d, %d).\n", i, j);
            }
        }
    }
    for (i = 0; i < 8; i++) {
        CuboidCorner c1 = cuboid->corners[i];
        CuboidCorner c2 = cb->corners[i];
        if (c1.index != c2.index || c1.symmetry != c2.symmetry) {
            printf("Error: corner mismatch at %d.\n", i);
        }
    }
}

int test_algorithm_equality(Algorithm * a1, Algorithm * a2) {
    if (a1->type != a2->type || a1->type != AlgorithmTypeContainer) {
        puts("Error: algorithms are not both containers.");
        return 0;
    }
    if (algorithm_container_count(a1) != algorithm_container_count(a2)) {
        puts("Error: algorithm lengths do not match.");
        return 0;
    }
    
    int i, retVal = 1;;
    for (i = 0; i < algorithm_container_count(a1); i++) {
        Algorithm * t1 = algorithm_container_get(a1, i);
        Algorithm * t2 = algorithm_container_get(a2, i);
        if (memcmp(t1, t2, sizeof(Algorithm)) != 0) {
            printf("Error: algorithms differ at index %d.\n", i);
            retVal = 0;
        }
    }
    return retVal;
}

int test_data_list_node_equality(DataListNode * n1, DataListNode * n2) {
    if (n1->subnodeCount != n2->subnodeCount) {
        puts("Error: subnodeCount does not match.");
        return 0;
    }
    if (n1->dataSize != n2->dataSize) {
        puts("Error: dataSize does not match.");
        return 0;
    }
    if (n1->depth != n2->depth) {
        puts("Error: depth does not match.");
        return 0;
    }
    if (n1->nodeByte != n2->nodeByte) {
        puts("Error: nodeByte does not match.");
        return 0;
    }
    int i;
    for (i = 0; i < n1->subnodeCount; i++) {
        DataListNode * s1 = (DataListNode *)n1->subnodes[i];
        DataListNode * s2 = (DataListNode *)n2->subnodes[i];
        if (!test_data_list_node_equality(s1, s2)) {
            return 0;
        }
    }
    if (memcmp(n1->nodeData, n2->nodeData, n1->dataSize)) {
        puts("Error: nodeData differs.");
        return 0;
    }
    return 1;
}
