#include "test.h"
#include "stickers/mapconversion.h"
#include "notation/cuboid.h"
#include "notation/parser.h"

void test_evil_eyes();
void test_redbull_algorithm();
void test_random_3x3();
void test_random_4x4();
void test_equivalent_4x4();
void test_superflip();
void test_3x4x3_line();

int main() {
    test_evil_eyes();
    test_redbull_algorithm();
    test_random_3x3();
    test_random_4x4();
    test_equivalent_4x4();
    test_superflip();
    test_3x4x3_line();
    
    tests_completed();
    return 0;
}

void test_evil_eyes() {
    test_initiated("evil eyes on an 11x11x11");
    
    CuboidDimensions dims = {11, 11, 11};
    Algorithm * algo = algorithm_for_string("R2 U2 R2 U2 R2 U2");
    Cuboid * evilEyes = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    
    int i, j;
    // verify solved centers
    for (i = 1; i <= 6; i++) {
        for (j = 0; j < 9*9; j++) {
            int centIndex = cuboid_center_index(evilEyes, i, j);
            CuboidCenter c = evilEyes->centers[centIndex];
            if (c.side != i) {
                printf("Error: invalid center at (%d, %d).\n", i, j);
            }
        }
    }
    // verify solved corners
    for (i = 0; i < 8; i++) {
        CuboidCorner c = evilEyes->corners[i];
        if (c.index != i || c.symmetry != 0) {
            printf("Error: invalid corner at index %d.\n", i);
        }
    }
    for (i = 0; i < 12; i++) {
        int expectedDedge = i;
        if (i == 0) expectedDedge = 6;
        if (i == 6) expectedDedge = 0;
        if (i == 1) expectedDedge = 7;
        if (i == 7) expectedDedge = 1;
        int dedgeCount = cuboid_count_edges_for_dedge(evilEyes, i);
        for (j = 0; j < dedgeCount; j++) {
            int index = cuboid_edge_index(evilEyes, i, j);
            CuboidEdge edge = evilEyes->edges[index];
            if (edge.symmetry != 0 || edge.dedgeIndex != expectedDedge) {
                printf("Error: invalid edge at (%d, %d).\n", i, j);
            }
        }
    }
    
    cuboid_free(evilEyes);
    test_completed();
}

void test_redbull_algorithm() {
    test_initiated("redbull on a 4x4x4");
    
    CuboidDimensions dims = {4, 4, 4};
    Algorithm * algo = algorithm_for_string("Rw2 R2 B2 U2 Lw L' U2 Rw' R U2 Rw R' U2 F2 Rw R' F2 Lw' L B2 Rw2 R2");
    Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    
    // verify corner identity
    int i, j;
    for (i = 0; i < 8; i++) {
        CuboidCorner c = cuboid->corners[i];
        if (c.index != i || c.symmetry != 0) {
            printf("Error: invalid corner at index %d.\n", i);
        }
    }
    
    // verify center identity
    for (i = 1; i <= 6; i++) {
        for (j = 0; j < 4; j++) {
            int index = cuboid_center_index(cuboid, i, j);
            CuboidCenter c = cuboid->centers[index];
            if (c.side != i) {
                printf("Error: invalid center at (%d, %d).\n", i, j);
            }
        }
    }
    
    // verify edges
    for (i = 0; i < 12; i++) {
        int edgeCount = cuboid_count_edges_for_dedge(cuboid, i);
        for (j = 0; j < edgeCount; j++) {
            int index = cuboid_edge_index(cuboid, i, j);
            CuboidEdge edge = cuboid->edges[index];
            if (i == 0) {
                if (edge.symmetry != 2 || edge.dedgeIndex != i || edge.edgeIndex != 1 - j) {
                    printf("Error: invaild edge (%d, %d).\n", i, j);
                }
            } else {
                if (edge.symmetry != 0 || edge.dedgeIndex != i || edge.edgeIndex != j) {
                    printf("Error: invalid edge (%d, %d).\n", i, j);
                }
            }
        }
    }
    
    cuboid_free(cuboid);
    test_completed();
}

void test_random_3x3() {
    test_initiated("a random 3x3x3 scramble");
    CuboidDimensions dim = {3, 3, 3};
    
    Algorithm * algo = algorithm_for_string("D L2 D2 R2 B2 D' F2 R' F2 L' R' U' R2 U2 L2 U2 B D' U2 L' D2 B U'");
    Cuboid * testCuboid = algorithm_to_cuboid(algo, dim);
    algorithm_free(algo);
    StickerMap * testMap = stickermap_create(dim);
    convert_cb_to_sm(testMap, testCuboid);
    cuboid_free(testCuboid);
    
    // scrambled with D L2 D2 R2 B2 D' F2 R' F2 L' R' U' R2 U2 L2 U2 B D' U2 L' D2 B U' (23 HTM)
    const char * stickerData = "321312451" "166126216" "445436546" "524542453" "313653536" "212365142";
    int i;
    for (i = 0; i < 9 * 6; i++) {
        int value = stickerData[i] - '1' + 1;
        if (testMap->stickers[i] != value) {
            printf("Error: invalid sticker at index %d, %d != %d.\n", i, value, testMap->stickers[i]);
        }
    }
    
    stickermap_free(testMap);
    test_completed();
}

void test_random_4x4() {
    test_initiated("a random 4x4x4 scramble");
    CuboidDimensions dim = {4, 4, 4};
    
    Algorithm * algo = algorithm_for_string("F U Uw R E' L2 Fw2 3Rw'");
    Cuboid * testCuboid = algorithm_to_cuboid(algo, dim);
    algorithm_free(algo);
    
    if (!testCuboid) {
        puts("Error: failed to read cuboid.");
    }
    
    StickerMap * testMap = stickermap_create(dim);
    convert_cb_to_sm(testMap, testCuboid);
    cuboid_free(testCuboid);
    
    //  U Uw R E' L2 Fw2 3Rw'
    const char * stickerData = "4662633564435556" "1441644543354332" "5226466621112113" "1111125132543222" "6424262255163316" "4655412345635533";
    
    int i;
    for (i = 0; i < 16 * 6; i++) {
        int value = stickerData[i] - '1' + 1;
        if (testMap->stickers[i] != value) {
            printf("Error: invalid sticker at index %d, %d != %d.\n", i, value, testMap->stickers[i]);
        }
    }
    
    stickermap_free(testMap);
    test_completed();
}

void test_equivalent_4x4() {
    test_initiated("Rw equivalency on a 4x4x4.");
    
    CuboidDimensions dim = {4, 4, 4};
    
    Algorithm * algo = algorithm_for_string("Rw2");
    Cuboid * testCuboid = algorithm_to_cuboid(algo, dim);
    algorithm_free(algo);
        
    Cuboid * control = cuboid_half_face_turn(dim, CuboidMovesAxisX, 1);
    Cuboid * slice = cuboid_half_slice(dim, CuboidMovesAxisX, 1);
    cuboid_multiply_to(slice, control);
    cuboid_free(slice);
    
    // test edge equivalency
    int i, j;
    for (i = 0; i < 12; i++) {
        for (j = 0; j < 2; j++) {
            int index = j + (i * 2);
            CuboidEdge e1 = control->edges[index];
            CuboidEdge e2 = testCuboid->edges[index];
            if (e1.edgeIndex != e2.edgeIndex || e1.symmetry != e2.symmetry ||
                e1.dedgeIndex != e2.dedgeIndex) {
                printf("Error: differnce in edge (%d, %d).\n", i, j);
            }
        }
    }
    // test corner equivalency
    for (i = 0; i < 8; i++) {
        CuboidCorner c1 = control->corners[i];
        CuboidCorner c2 = testCuboid->corners[i];
        if (c1.index != c2.index || c1.symmetry != c2.symmetry) {
            printf("Error: difference in corner %d.\n", i);
        }
    }
    // test center equivalency
    for (i = 1; i <= 6; i++) {
        for (j = 0; j < 4; j++) {
            int index = 4 * (i - 1) + j;
            CuboidCenter c1 = control->centers[index];
            CuboidCenter c2 = testCuboid->centers[index];
            if (c1.side != c2.side || c1.index != c2.index) {
                printf("Error: difference in center (%d, %d).\n", i, j);
            }
        }
    }
        
    cuboid_free(control);
    cuboid_free(testCuboid);
    
    test_completed();
}

void test_superflip() {
    test_initiated("the superflip on a 3x3x3");
    
    CuboidDimensions dims = {3, 3, 3};
    Algorithm * algo = algorithm_for_string("(M U' M U' M U' M U' y z')3");
    Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    
    // ensure corner identity
    int i;
    for (i = 0; i < 8; i++) {
        CuboidCorner c = cuboid->corners[i];
        if (c.index != i || c.symmetry != 0) {
            printf("Error: invalid corner at index %d.\n", i);
        }
    }
    // ensure center identity
    for (i = 0; i < 6; i++) {
        CuboidCenter c = cuboid->centers[i];
        if (c.side != i + 1 || c.index != 0) {
            printf("Error: invalid center at index %d.\n", i);
        }
    }
    // ensure edge psuedo-identity
    for (i = 0; i < 12; i++) {
        CuboidEdge edge = cuboid->edges[i];
        if (edge.dedgeIndex != i || edge.symmetry == 0) {
            printf("Error: invalid edge at index %d.\n", i);
        }
    }
    
    cuboid_free(cuboid);
    test_completed();
}

void test_3x4x3_line() {
    test_initiated("3x4x3 \"line\" algorithm");
    
    CuboidDimensions dims = {3, 4, 3};
    const char * data = "212212212212" "121121121121" "333333333" "444444444" "666666666666" "555555555555";
    StickerMap * map = stickermap_create(dims);
    int i;
    for (i = 0; i < 3*3*2 + 3*4*4; i++) {
        map->stickers[i] = data[i] - '1' + 1;
    }
    Cuboid * cuboid = cuboid_create(dims);
    if (!convert_sm_to_cb(cuboid, map)) {
        puts("Error: failed to read stickermap :'(");
    }
    stickermap_free(map);
    
    Algorithm * algo = algorithm_for_string("R2 L2 Uw2 R2 L2 Uw2");
    Cuboid * compare = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    
    for (i = 0; i < 8; i++) {
        CuboidCorner c1 = compare->corners[i];
        CuboidCorner c2 = cuboid->corners[i];
        if (c1.index != c2.index || c1.symmetry != c2.symmetry) {
            printf("Error: corners differ at index %d.\n", i);
            printf("(expected %d, found %d)\n", c2.index, c1.index);
        }
    }
    for (i = 0; i < cuboid_count_edges(cuboid); i++) {
        CuboidEdge e1 = compare->edges[i];
        CuboidEdge e2 = cuboid->edges[i];
        if (e1.dedgeIndex != e2.dedgeIndex || 
            e1.symmetry != e2.symmetry) {
                printf("Error: edges differ at index %d.\n", i);
        }
    }
    for (i = 0; i < cuboid_count_centers(cuboid); i++) {
        CuboidCenter c1 = compare->centers[i];
        CuboidCenter c2 = cuboid->centers[i];
        if (c1.side != c2.side) {
            printf("Error: centers differ at index %d.\n", i);
        }
    }
    
    cuboid_free(compare);
    cuboid_free(cuboid);
    test_completed();
}
