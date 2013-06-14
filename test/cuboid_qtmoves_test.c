#include "representation/cuboid_qtmoves.h"
#include "test.h"

void test_validate();
void test_2x2_turns();
void test_4x4_turns();
void test_slices_3x3();
void test_slices_4x4();
void test_slices_7x7();

int main() {
    test_validate();
    test_2x2_turns();
    test_4x4_turns();
    test_slices_3x3();
    test_slices_4x4();
    test_slices_7x7();
    
    tests_completed();
    return 0;
}

void test_validate() {
    test_initiated("cuboid_validate_quarter_turn()");
    CuboidDimensions dim;
    dim.x = 3;
    dim.y = 4;
    dim.z = 3;
    if (!cuboid_validate_quarter_turn(dim, CuboidMovesAxisY)) {
        puts("Error: on a 3x4x3 a Y quarter turn is allowed.");
    }
    if (cuboid_validate_quarter_turn(dim, CuboidMovesAxisX)) {
        puts("Error: on a 3x4x3 an X quarter turn is not allowed.");
    }
    if (cuboid_validate_quarter_turn(dim, CuboidMovesAxisZ)) {
        puts("Error: on a 3x4x3 a Z quarter turn is not allowed.");
    }
    test_completed();
}

void test_2x2_turns() {
    test_initiated("turns on the 2x2x2");
    CuboidDimensions dim;
    dim.x = 2;
    dim.y = 2;
    dim.z = 2;
    
    Cuboid * right = cuboid_quarter_face_turn(dim, CuboidMovesAxisX, 1);
    Cuboid * up = cuboid_quarter_face_turn(dim, CuboidMovesAxisY, 1);

    const uint8_t correctRightIndexes[] = {0, 1, 2, 3, 5, 7, 4, 6};
    const uint8_t correctUpIndexes[] = {0, 1, 3, 7, 4, 5, 2, 6};
    int i;
    for (i = 0; i < 8; i++) {
        CuboidCorner upCorner = up->corners[i];
        CuboidCorner rightCorner = right->corners[i];
        if (upCorner.index != correctUpIndexes[i]) {
            printf("Error: invalid up corner at index %d\n", i);
        }
        if (rightCorner.index != correctRightIndexes[i]) {
            printf("Error: invalid right corner at index %d\n", i);
        }
    }
    
    // confirm that (R2 U2)3 = identity
    Cuboid * cube = cuboid_create(dim);
    for (i = 0; i < 3; i++) {
        Cuboid * output = cuboid_create(dim);
        cuboid_multiply(output, right, cube);
        cuboid_multiply(cube, right, output);
        cuboid_multiply(output, up, cube);
        cuboid_multiply(cube, up, output);
        cuboid_free(output);
    }
    
    for (i = 0; i < 8; i++) {
        if (cube->corners[i].index != i || cube->corners[i].symmetry != 0) {
            printf("Error: invalid (R2 U2)3 corner at index %d\n", i);
        }
    }
    
    cuboid_free(cube);
    cuboid_free(right);
    cuboid_free(up);
    
    test_completed();
}

void test_4x4_turns() {
    test_initiated("turns on the 4x4x4");
    
    CuboidDimensions dim = {4, 4, 4};
    
    Cuboid * down = cuboid_quarter_face_turn(dim, CuboidMovesAxisY, -1);
    Cuboid * back = cuboid_quarter_face_turn(dim, CuboidMovesAxisZ, -1);
    
    // validate the centers
    uint8_t expectedCenters[] = {1, 3, 0, 2};
    int i;
    for (i = 0; i < 4; i++) {
        int idxDown = cuboid_center_index(down, 4, i);
        int idxBack = cuboid_center_index(back, 2, i);
        CuboidCenter center = down->centers[idxDown];
        if (center.side != 4 || center.index != expectedCenters[i]) {
            printf("Error: invalid down center at index %d: (side=%d, index=%d)\n",
                    i, center.side, center.index);
        }
        center = back->centers[idxBack];
        if (center.side != 2 || center.index != expectedCenters[i]) {
            printf("Error: invalid back center at index %d\n", i);
        }
    }
    
    // quickly validate edges on the down face
    
    // contains {newDedge, flip flag}
    uint8_t edges[4][2] = {{11, 0}, {8, 1}, {10, 0}, {2, 1}};
    uint8_t physicalDedgeSlots[4] = {2, 11, 8, 10};
    
    for (i = 0; i < 4; i++) {
        uint8_t dedge = physicalDedgeSlots[i];
        uint16_t edgeIndex = cuboid_edge_index(down, dedge, 0);
        CuboidEdge physicalEdge = down->edges[edgeIndex];
        if (physicalEdge.dedgeIndex != edges[i][0]) {
            printf("Error: invalid down turn dedge at dedge index %d, found %d\n",
                   (int)dedge, physicalEdge.dedgeIndex);
        }
        if (physicalEdge.edgeIndex != edges[i][1]) {
            printf("Error: invalid down turn edge index at dedge %d\n", (int)dedge);
        }
    }
    
    Cuboid * cube = cuboid_create(dim);
    
    // ensure that double evil eyes yields the identity for centers and edges
    for (i = 0; i < 6; i++) {
        Cuboid * output = cuboid_create(dim);
        cuboid_multiply(output, down, cube);
        cuboid_multiply(cube, down, output);
        cuboid_multiply(output, back, cube);
        cuboid_multiply(cube, back, output);
        cuboid_free(output);
    }
    
    for (i = 1; i <= 6; i++) {
        int j;
        for (j = 0; j < 4; j++) {
            int centerIndex = cuboid_center_index(cube, i, j);
            CuboidCenter c = cube->centers[centerIndex];
            if (c.side != i || c.index != j) {
                printf("Error: evil eyes does not yield ID for center on face %d.\n", i);
            }
        }
    }
    for (i = 0; i < 12; i++) {
        int edge0Index = cuboid_edge_index(cube, i, 0);
        int edge1Index = cuboid_edge_index(cube, i, 1);
        CuboidEdge edge = cube->edges[edge0Index];
        if (edge.symmetry != 0 || edge.edgeIndex != 0 || edge.dedgeIndex != i) {
            printf("Error: evil eyes does not yield ID for dedge %d edge 0.\n", i);
        }
        edge = cube->edges[edge1Index];
        if (edge.symmetry != 0 || edge.edgeIndex != 1 || edge.dedgeIndex != i) {
            printf("Error: evil eyes does not yield ID for dedge %d edge 1.\n", i);
        }
    }
    for (i = 0; i < 8; i++) {
        if (cube->corners[i].index != i || cube->corners[i].symmetry != 0) {
            printf("Error: evil eyes does not yield ID for corner at index %d\n", i);
        }
    }
    
    cuboid_free(down);
    cuboid_free(back);
    cuboid_free(cube);
    
    test_completed();
}

void test_slices_3x3() {
    test_initiated("slices on a 3x3x3");
    
    CuboidDimensions dim = {3, 3, 3};
    
    Cuboid * sliceM = cuboid_quarter_slice(dim, CuboidMovesAxisX, 0);
    Cuboid * sliceS = cuboid_quarter_slice(dim, CuboidMovesAxisZ, 0);
    Cuboid * sliceEPrime = cuboid_quarter_slice(dim, CuboidMovesAxisY, 0);
    
    // validate center movement
    uint8_t centersS[] = {1, 2, 6, 5, 3, 4};
    uint8_t centersM[] = {3, 4, 2, 1, 5, 6};
    uint8_t centersEPrime[] = {5, 6, 3, 4, 2, 1};
    int i;
    for (i = 0; i < 6; i++) {
        if (sliceM->centers[i].side != centersM[i]) {
            printf("Error: invalid M center at index %d\n", i);
        }
        if (sliceEPrime->centers[i].side != centersEPrime[i]) {
            printf("Error: invalid E center at index %d\n", i);
        }
        if (sliceS->centers[i].side != centersS[i]) {
            printf("Error: invalid S center at index %d\n", i);
        }
    }
    
    // do M S2 M3 S2
    Cuboid * cube = cuboid_create(dim);
    cuboid_multiply_to(sliceM, cube);
    cuboid_multiply_to(sliceS, cube);
    cuboid_multiply_to(sliceS, cube);
    cuboid_multiply_to(sliceM, cube);
    cuboid_multiply_to(sliceM, cube);
    cuboid_multiply_to(sliceM, cube);
    cuboid_multiply_to(sliceS, cube);
    cuboid_multiply_to(sliceS, cube);
    
    // ensure corner stability
    for (i = 0; i < 8; i++) {
        CuboidCorner corner = cube->corners[i];
        if (corner.symmetry != 0 || corner.index != i) {
            printf("Error: invalid M S2 M3 S2 corner at %d\n", i);
        }
    }
    for (i = 0; i < 12; i++) {
        CuboidEdge edge = cube->edges[i];
        if (edge.symmetry != 0 || edge.dedgeIndex != i) {
            printf("Error: invalid M S2 M3 S2 edge at %d\n", i);
        }
    }
    
    uint8_t expectedCenters[6] = {2, 1, 4, 3, 5, 6};
    for (i = 0; i < 6; i++) {
        if (cube->centers[i].side != expectedCenters[i]) {
            printf("Error: invalid M S2 M3 S2 center at %d\n", i + 1);
        }
    }
    
    cuboid_free(sliceM);
    cuboid_free(sliceS);
    cuboid_free(sliceEPrime);
    cuboid_free(cube);
    
    test_completed();
}

void test_slices_4x4() {
    test_initiated("slices on the 4x4x4");
    
    CuboidDimensions dim = {4, 4, 4};
    Cuboid * upperEPrime = cuboid_quarter_slice(dim, CuboidMovesAxisY, 0);
    Cuboid * rightM = cuboid_quarter_slice(dim, CuboidMovesAxisX, 1);
    
    Cuboid * cuboid = cuboid_create(dim);
    int i;
    for (i = 0; i < 2; i++) {
        cuboid_multiply_to(rightM, cuboid);
        cuboid_multiply_to(rightM, cuboid);
        cuboid_multiply_to(upperEPrime, cuboid);
        cuboid_multiply_to(upperEPrime, cuboid);
    }
    
    // verify the centers
    uint8_t centers[6][4] = {
        {2, 2, 1, 1}, {1, 1, 2, 2}, {3, 3, 3, 3}, {4, 4, 4, 4},
        {5, 5, 5, 5}, {6, 6, 6, 6}
    };
    for (i = 1; i <= 6; i++) {
        int j;
        for (j = 0; j < 4; j++) {
            int index = cuboid_center_index(cuboid, i, j);
            CuboidCenter c = cuboid->centers[index];
            if (c.side != centers[i - 1][j]) {
                printf("Error: invalid center at (%d, %d)\n", i, j);
            }
        }
    }
    
    cuboid_free(upperEPrime);
    cuboid_free(rightM);
    cuboid_free(cuboid);
    
    puts("Test complete.");
}

void test_slices_7x7() {
    puts("Testing slices on the 7x7x7...");
    
    CuboidDimensions dim = {7, 7, 7};
    Cuboid * innerRightM = cuboid_quarter_slice(dim, CuboidMovesAxisX, 3);
    Cuboid * innerUpEPrime = cuboid_quarter_slice(dim, CuboidMovesAxisY, 1);
    
    Cuboid * cuboid = cuboid_create(dim);
    int i;
    for (i = 0; i < 2; i++) {
        cuboid_multiply_to(innerRightM, cuboid);
        cuboid_multiply_to(innerRightM, cuboid);
        cuboid_multiply_to(innerUpEPrime, cuboid);
        cuboid_multiply_to(innerUpEPrime, cuboid);
    }
    
    // centers at index 6 and 8 on faces 1 & 2 appear swapped
    for (i = 0; i < 25; i++) {
        int index1 = cuboid_center_index(cuboid, 1, i);
        int index2 = cuboid_center_index(cuboid, 2, i);
        CuboidCenter c1 = cuboid->centers[index1];
        CuboidCenter c2 = cuboid->centers[index2];
        if (i == 6 || i == 8) {
            if (c1.side != 2 || c2.side != 1) {
                printf("Error: invalid color at index %d\n", i);
            }
        } else if (c1.side != 1 || c2.side != 2) {
            printf("Error: invalid color at index %d\n", i);
        }
        int j;
        for (j = 3; j <= 6; j++) {
            int index = cuboid_center_index(cuboid, j, i);
            CuboidCenter center = cuboid->centers[index];
            if (center.side != j) {
                printf("Error: invalid color at (%d, %d)\n", j, i);
            }
        }
    }
    
    cuboid_free(cuboid);
    cuboid_free(innerRightM);
    cuboid_free(innerUpEPrime);
    test_completed();
}
