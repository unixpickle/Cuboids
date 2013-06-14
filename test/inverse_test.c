#include "test.h"
#include "representation/cuboid_qtmoves.h"
#include "algebra/inverse.h"

void test_inverse();
void ensure_identity(Cuboid * cuboid);

int main() {
    test_inverse();
    
    tests_completed();
    return 0;
}

void test_inverse() {
    test_initiated("the inverse law");
    
    CuboidDimensions dims = {4, 4, 4};
    
    // test left application of inverse w/ face turn
    Cuboid * turn = cuboid_quarter_face_turn(dims, CuboidMovesAxisX, 1);
    Cuboid * inverse = cuboid_inverse(turn);
    
    cuboid_multiply_to(inverse, turn);
    ensure_identity(turn);
    
    cuboid_free(turn);
    cuboid_free(inverse);
    
    // test right application of inverse w/ slices
    turn = cuboid_quarter_slice(dims, CuboidMovesAxisY, 1);
    inverse = cuboid_inverse(turn);
    
    cuboid_multiply_to(turn, inverse);
    ensure_identity(inverse);
    
    cuboid_free(turn);
    cuboid_free(inverse);
    
    test_completed();
}

void ensure_identity(Cuboid * cuboid) {
    int i, j;
    for (i = 0; i < 12; i++) {
        for (j = 0; j < 2; j++) {
            CuboidEdge edge = cuboid->edges[cuboid_edge_index(cuboid, i, j)];
            if (edge.dedgeIndex != i || edge.edgeIndex != j) {
                printf("Error: invalid inverse edge at (%d, %d).\n", i, j);
            }
        }
    }
    
    for (i = 0; i < 8; i++) {
        CuboidCorner corner = cuboid->corners[i];
        if (corner.index != i || corner.symmetry != 0) {
            printf("Error: invalid inverse corner at index %d.\n", i);
        }
    }
    
    for (i = 1; i <= 6; i++) {
        for (j = 0; j < 4; j++) {
            CuboidCenter center = cuboid->centers[cuboid_center_index(cuboid, i, j)];
            if (center.side != i || center.index != j) {
                printf("Error: invalid inverse center at (%d, %d).\n", i, j);
            }
        }
    }
}
