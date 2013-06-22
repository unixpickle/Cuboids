#include "test.h"
#include "search/cuboid.h"
#include "stickers/mapconversion.h"
#include "algebra/rotation_group.h"

static int solution[16];
static volatile unsigned long long cubesFound;

void test_solve_3x3();

void handle_cuboid(void * data, const Cuboid * cuboid, Cuboid * cache,
                   const int * sequence, int len);

int main() {
    test_solve_3x3();
    
    tests_completed();
}

void test_solve_3x3() {
    test_initiated("brute force 3x3");
    
    cubesFound = 0;
    CuboidDimensions dims = {3, 3, 3};
    
    Algorithm * scramble = algorithm_for_string("R2 D' B U2 L'");
    Cuboid * solveMe = algorithm_to_cuboid(scramble, dims);
    algorithm_free(scramble);
    
    AlgList * list = alg_list_parse("R,U,L,F,B,D,R',U',L',F',D',B',R2,U2,L2,D2,F2,B2", dims);
    
    CSSettings settings;
    CSCallbacks callbacks;
    BSSettings bsSettings;
    
    // these two arguments are consumed and freed by the solver
    settings.rootNode = solveMe;
    settings.algorithms = list;
    settings.cacheCuboid = 0;
    
    bsSettings.threadCount = 8;
    bsSettings.minDepth = 1;
    bsSettings.maxDepth = 5;
    bsSettings.nodeInterval = 1000000;
    
    RotationBasis basis = rotation_basis_standard(dims);
    RotationGroup * group = rotation_group_create_basis(basis);
    
    bzero(&callbacks, sizeof(callbacks));
    callbacks.handle_cuboid = handle_cuboid;
    callbacks.userData = group;
    
    CSSearchContext * search = cs_run(settings, bsSettings, callbacks);
    while (1) {
        usleep(100000);
        if (!cs_context_is_running(search)) {
            break;
        }
    }
    
    BSProgress progress = bs_context_progress(search->bsContext);
    cs_context_release(search);
    
    int magicalSequence[5] = {2, 13, 11, 5, 12};
    if (memcmp(solution, magicalSequence, 5) != 0) {
        puts("Error: solution does not match correct one.");
    }
    
    printf("Stats: nodesExpanded = %lld\n", progress.nodesExpanded);
    
    if (cubesFound != 2000718) {
        printf("Error: found %lld cubes, expected 2000718.\n", cubesFound);
    }
    
    rotation_group_release(group);
    test_completed();
}

void handle_cuboid(void * data, const Cuboid * cuboid, Cuboid * _cache,
                   const int * sequence, int len) {
    __sync_add_and_fetch(&cubesFound, 1);
    assert(!_cache);
    
    RotationGroup * group = (RotationGroup *)data;
    int isSolved = rotation_group_contains(group, cuboid);
    
    if (isSolved) {
        memcpy(solution, sequence, len * sizeof(int));
    }
}
