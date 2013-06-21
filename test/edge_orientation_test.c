#include "test.h"
#include "pieces/eo.h"
#include "notation/cuboid.h"
#include "notation/parser.h"

void test_superflip();
void test_rl_orientation();
void test_ud_orientation();
void test_fb_orientation();

int main(int argc, const char * argv[]) {
    test_superflip();
    test_rl_orientation();
    test_ud_orientation();
    test_fb_orientation();
    
    tests_completed();
    return 0;
}

void test_superflip() {
    test_initiated("edge orientations on superflip");
    
    CuboidDimensions dims = {3, 3, 3};
    Algorithm * algo = algorithm_for_string("(M U' M U' M U' M U' y z')3");
    Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    
    int i;
    for (i = 0; i < 12; i++) {
        CuboidEdge edge = cuboid->edges[i];
        int orX = cuboid_edge_orientation(edge, i, 0);
        int orY = cuboid_edge_orientation(edge, i, 0);
        int orZ = cuboid_edge_orientation(edge, i, 0);
        if (orX) {
            printf("Error: edge %d claims to be oriented on the X axis.\n", i);
        }
        if (orY) {
            printf("Error: edge %d claims to be oriented on the Y axis.\n", i);
        }
        if (orZ) {
            printf("Error: edge %d claims to be oriented on the Z axis.\n", i);
        }
    }
    
    cuboid_free(cuboid);
    test_completed();
}

void test_rl_orientation() {
    test_initiated("R & L edge orientation");
    
    CuboidDimensions dims = {3, 3, 3};
    Algorithm * algo = algorithm_for_string("R U2 D' F' U2 L' B2 D' F");
    Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
    uint8_t eoMap[12] = {0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0};
    int i;
    
    for (i = 0; i < 12; i++) {
        CuboidEdge edge = cuboid->edges[i];
        uint8_t flag = cuboid_edge_orientation(edge, i, 0);
        if (flag != eoMap[i]) {
            printf("Error: invalid EO at %d.\n", i);
        }
    }
    
    cuboid_free(cuboid);
    test_completed();
}

void test_ud_orientation() {
    test_initiated("U & D edge orientation");
    
    CuboidDimensions dims = {3, 3, 3};
    Algorithm * algo = algorithm_for_string("R D2 F L' U2 D' F2 L' B");
    Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
    uint8_t eoMap[12] = {0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0};
    int i;
    
    for (i = 0; i < 12; i++) {
        CuboidEdge edge = cuboid->edges[i];
        uint8_t flag = cuboid_edge_orientation(edge, i, 1);
        if (flag != eoMap[i]) {
            printf("Error: invalid EO at %d.\n", i);
        }
    }
    
    cuboid_free(cuboid);
    test_completed();
}

void test_fb_orientation() {
    test_initiated("F & B edge orientation");
    
    CuboidDimensions dims = {3, 3, 3};
    Algorithm * algo = algorithm_for_string("R D2 F L' U2 D' F2 L' B");
    Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
    uint8_t eoMap[12] = {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1}; // I entered this SO fast
    int i;
    
    for (i = 0; i < 12; i++) {
        CuboidEdge edge = cuboid->edges[i];
        uint8_t flag = cuboid_edge_orientation(edge, i, 2);
        if (flag != eoMap[i]) {
            printf("Error: invalid EO at %d.\n", i);
        }
    }
    
    cuboid_free(cuboid);
    test_completed();
}
