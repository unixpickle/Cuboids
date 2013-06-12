#include "cuboid_qtmoves.h"
#include <stdio.h>

void test_validate();
void test_2x2_turns();
void test_4x4_turns();

int main() {
    test_validate();
    test_2x2_turns();
    test_4x4_turns();
    
    printf("Hit return to exit...");
    char buf[3];
    fgets(buf, 3, stdin);
    
    return 0;
}

void test_validate() {
    puts("Testing cuboid_validate_quarter_turn()...");
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
    puts("Test complete.");
}

void test_2x2_turns() {
    puts("Testing turns on the 2x2x2...");
    CuboidDimensions dim;
    dim.x = 2;
    dim.y = 2;
    dim.z = 2;
    
    Cuboid * right = cuboid_generate_quarter_face_turn(dim, CuboidMovesAxisX, 1);
    Cuboid * up = cuboid_generate_quarter_face_turn(dim, CuboidMovesAxisY, 1);

    puts("Validating R and U...");

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
    
    puts("Test complete.");
}

void test_4x4_turns() {
    puts("Testing turns on the 4x4x4...");
    
    CuboidDimensions dim = {4, 4, 4};
    
    Cuboid * down = cuboid_generate_quarter_face_turn(dim, CuboidMovesAxisY, -1);
    Cuboid * back = cuboid_generate_quarter_face_turn(dim, CuboidMovesAxisZ, -1);
    
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
    
    puts("Test complete.");
}
