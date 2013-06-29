#include "pieces/co.h"
#include "test.h"

#define CODATA(c1,c2,c3,c4,c5,c6,c7,c8) (c1 | (c2 << 2) | (c3 << 4) | (c4 << 6) | (c5 << 8) |\
(c6 << 12) | (c7 << 14) | (c8 << 16))

void test_scramble();
void test_axis_compatibility();

int main(int argc, const char * argv[]) {
    test_scramble();
    test_axis_compatibility();
    
    tests_completed();
    return 0;
}

void test_scramble() {
    test_initiated("CO on a random scramble");
    
    CuboidDimensions dims = {2, 2, 2};
    Algorithm * algo = algorithm_for_string("R2 U' D2 L B' F U D2 B L R F'");
    Cuboid * cb = algorithm_to_cuboid(algo, dims);
    
    uint16_t xOrientations[] = {1, 1, 1, 2, 0, 0, 2, 0};
    uint16_t yOrientations[] = {0, 2, 2, 1, 2, 1, 0, 2};
    uint16_t zOrientations[] = {2, 0, 0, 0, 1, 2, 1, 1};
    int i;
    
    for (i = 0; i < 8; i++) {
        uint8_t xOr = cuboid_corner_orientation(cb->corners[i].symmetry, 0);
        uint8_t yOr = cuboid_corner_orientation(cb->corners[i].symmetry, 1);
        uint8_t zOr = cuboid_corner_orientation(cb->corners[i].symmetry, 2);
        if (xOr != xOrientations[i]) {
            printf("Error: invalid x orientation at index %d.\n", i);
        }
        if (yOr != yOrientations[i]) {
            printf("Error: invalid y orientation at index %d.\n", i);
        }
        if (zOr != zOrientations[i]) {
            printf("Error: invalid z orientation at index %d.\n", i);
        }
    }
    
    test_completed();
}

void test_axis_compatibility() {
    test_initiated("co_context_axis_compatibility");
    
    CuboidDimensions dims = {3, 3, 3};
    RotationBasis symmetries = {dims, 2, 1, 2};
    COContext * context = co_context_create(symmetries);
    if (context->x || context->z) {
        puts("Error: context should only allocate y rotation");
    }
    if (!co_context_axis_compatibility(context, 0, 2)) {
        puts("Error: x and z axis are compatible.");
    }
    if (co_context_axis_compatibility(context, 2, 1)) {
        puts("Error: y and z axis are not compatible.");
    }
    if (co_context_axis_compatibility(context, 0, 1)) {
        puts("Error: x and y axis are not compatible.");
    }
    
    co_context_free(context);
    test_completed();
}
