#include "representation/cuboid_base.h"
#include "test.h"

void test_2x2();
void test_big_cube();
void test_cuboid();

int main() {
    test_2x2();
    test_big_cube();
    test_cuboid();
    
    tests_completed();
    return 0;
}

void test_2x2() {
    test_initiated("the 2x2x2 cuboid");
    CuboidDimensions dim;
    dim.x = 2;
    dim.y = 2;
    dim.z = 2;
    Cuboid * ident = cuboid_create(dim);
    
    if (ident->edges) {
        puts("Error: 2x2x2 should not create edges");
    }
    if (ident->centers) {
        puts("Error: 2x2x2 should not create centers");
    }
    
    uint8_t correct[] = {0, 1, 2, 3, 4, 5, 6, 7};
    if (memcmp(ident->corners, correct, 8) != 0) {
        printf("Error: identity corner data is invalid:");
        int i;
        for (i = 0; i < 8; i++) {
            printf(" %d", ((uint8_t *)ident->corners)[i]);
        }
        printf("\n");
    }
    
    cuboid_free(ident);
    
    test_completed();
}

void test_big_cube() {
    test_initiated("the 8x8x8 cuboid");
    CuboidDimensions dim;
    int i, j;
    dim.x = 8;
    dim.y = 8;
    dim.z = 8;
    Cuboid * cuboid = cuboid_create(dim);
    
    if (!cuboid->edges) {
        puts("Error: 8x8x8 should create edges");
    }
    if (!cuboid->centers) {
        puts("Error: 8x8x8 should create centers");
    }
    
    // check dedges
    int lastIndex = -1;
    for (i = 0; i < 12; i++) {
        uint8_t edgeCount = cuboid_count_edges_for_dedge(cuboid, i);
        if (edgeCount != 6) {
            printf("Error: all dedges should be 6 long, but dedge %d is not.\n", i);
        }
        for (j = 0; j < edgeCount; j++) {
            uint16_t index = cuboid_edge_index(cuboid, i, j);
            if (index != lastIndex + 1) {
                printf("Error: edge indexes should increment by 1, but not for (%d, %d)\n", i, j);
            }
            lastIndex = index;
        }
    }
    
    // check centers
    lastIndex = -1;
    for (i = 1; i <= 6; i++) {
        uint8_t centerCount = cuboid_count_centers_for_face(cuboid, i);
        if (centerCount != 6*6) {
            printf("Error: all centers should contain 36 pieces, but center %d does not\n", i);
        }
        for (j = 0; j < centerCount; j++) {
            uint16_t index = cuboid_center_index(cuboid, i, j);
            if (index != lastIndex + 1) {
                printf("Error: center indexes should increment by 1, but not for (%d, %d)\n", i, j);
            }
            lastIndex = index;
        }
    }
    
    cuboid_free(cuboid);
    test_completed();
}

void test_cuboid() {
    test_initiated("the 3x4x3 cuboid");
    CuboidDimensions dim;
    dim.x = 3;
    dim.y = 4;
    dim.z = 3;
    Cuboid * cuboid = cuboid_create(dim);
    
    if (!cuboid->edges) {
        puts("Error: 3x4x3 should create edges");
    }
    if (!cuboid->centers) {
        puts("Error: 3x4x3 should create centers");
    }
    
    int i;
    for (i = 0; i < 12; i++) {
        int edgeCount = cuboid_count_edges_for_dedge(cuboid, i);
        if (i == 1 || i == 3 || i == 7 || i == 9) {
            if (edgeCount != 2) {
                puts("Error: the E slice should have two edges per dedge");
            }
        } else {
            if (edgeCount != 1) {
                puts("Error: the top and bottom layers should have single dedges");
            }
        }
    }
    
    cuboid_free(cuboid);
    test_completed();
}
