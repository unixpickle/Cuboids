#include "cuboid_htmoves.h"
#include <stdio.h>

void test_3x2x3_turns();
void test_5x5x5_turns();

int main() {
    test_3x2x3_turns();
    test_5x5x5_turns();
}

void test_3x2x3_turns() {
    puts("Testing 3x2x3 half-turns...");
    CuboidDimensions dims = {3, 2, 3};
    
    Cuboid * turnR = cuboid_half_face_turn(dims, CuboidMovesAxisX, 1);
    Cuboid * turnU = cuboid_half_face_turn(dims, CuboidMovesAxisY, 1);
    
    // validate R turn edges
    CuboidEdge edge1 = turnR->edges[cuboid_edge_index(turnR, 5, 0)];
    CuboidEdge edge2 = turnR->edges[cuboid_edge_index(turnR, 11, 0)];
    if (edge1.symmetry != 0 || edge1.dedgeIndex != 11) {
        puts("Error: dedge 5 is incorrect.\n");
    }
    if (edge2.symmetry != 0 || edge2.dedgeIndex != 5) {
        puts("Error: dedge 11 is incorrect.\n");
    }
    int i;
    for (i = 0; i < 12; i++) {
        if (i == 5 || i == 11) continue;
        if (cuboid_count_edges_for_dedge(turnR, i) != 1) {
            if (i == 1 || i == 3 || i == 7 || i == 9) continue;
            printf("Error: representation does not see edge at %d\n", i);
            continue;
        }
        CuboidEdge edge = turnR->edges[cuboid_edge_index(turnR, i, 0)];
        if (edge.dedgeIndex != i) {
            printf("Error: edge at %d is incorrect.\n", i);
        }
    }
    
    // validate R turn corners
    uint8_t expectedCorners[8] = {0, 1, 2, 3, 7, 6, 5, 4};
    for (i = 0; i < 8; i++) {
        if (turnR->corners[i].index != expectedCorners[i]) {
            printf("Error: wrong corner at index %d, got %d.\n", i, turnR->corners[i].index);
        }
    }
    
    // perform evil eyes and ensure proper swap
    Cuboid * cuboid = cuboid_create(dims);
    for (i = 0; i < 3; i++) {
        cuboid_multiply_to(turnR, cuboid);
        cuboid_multiply_to(turnU, cuboid);
    }
    
    edge1 = cuboid->edges[cuboid_edge_index(cuboid, 0, 0)];
    edge2 = cuboid->edges[cuboid_edge_index(cuboid, 6, 0)];
    if (edge1.dedgeIndex != 6) {
        printf("Error: invalid evil eyes dedge at index 0, actual is %d\n", edge1.dedgeIndex);
    }
    if (edge2.dedgeIndex != 0) {
        puts("Error: invalid evil eyes dedge at index 6");
    }
    
    for (i = 0; i < 12; i++) {
        if (i == 6 || i == 0) continue;
        if (cuboid_count_edges_for_dedge(turnR, i) != 1) continue;
        CuboidEdge edge = cuboid->edges[cuboid_edge_index(cuboid, i, 0)];
        if (edge.dedgeIndex != i) {
            printf("Error: wrong edge at index %d.\n", i);
        }
    }
    
    cuboid_free(cuboid);
    cuboid_free(turnR);
    cuboid_free(turnU);
    
    puts("Test complete.");
}

void test_5x5x5_turns() {
    puts("Testing 5x5x5 half-turns...");
    CuboidDimensions dims = {5, 5, 5};
    
    Cuboid * turnR = cuboid_half_face_turn(dims, CuboidMovesAxisX, 1);
    
    uint8_t rightCenter[] = {8, 7, 6, 5, 4, 3, 2, 1, 0};
    int i;
    for (i = 0; i < 9; i++) {
        CuboidCenter c = turnR->centers[cuboid_center_index(turnR, 5, i)];
        if (c.index != rightCenter[i]) {
            printf("Error: invalid right center at index %d.\n", i);
        }
        int j;
        for (j = 1; j <= 6; j++) {
            if (j == 5) continue;
            CuboidCenter cent = turnR->centers[cuboid_center_index(turnR, j, i)];
            if (cent.index != i || cent.side != j) {
                printf("Error: invalid center piece (%d, %d).\n", j, i);
            }
        }
    }
    
    // check the right dedges
    uint8_t checkEdges[4] = {1, 5, 7, 11};
    for (i = 0; i < 4; i++) {
        int j, dedgeIndex = checkEdges[i];
        int complementDedge = (i < 2 ? checkEdges[i + 2] : checkEdges[i - 2]);
        for (j = 0; j < 3; j++) {
            CuboidEdge edge = turnR->edges[cuboid_edge_index(turnR, dedgeIndex, j)];
            if (edge.edgeIndex != 3 - j - 1 ||
                edge.dedgeIndex != complementDedge) {
                printf("Error: invalid edge at (%d, %d), got (%d, %d).\n", 
                    dedgeIndex, j, edge.dedgeIndex, edge.dedgeIndex);
            }
        }
    }
    
    cuboid_free(turnR);
    puts("Test complete.");
}
