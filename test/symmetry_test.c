#include "symmetry/rotation_cosets.h"
#include "test.h"

void test_rotation_group_counts();
void test_rotation_group_cuboids();
void test_rotation_cosets();

int main(int argc, const char * argv[]) {
    test_rotation_group_counts();
    
    tests_completed();
    return 0;
}

void test_rotation_group_counts() {
    test_initiated("orders of common rotation groups");
    
    CuboidDimensions dims = {7, 7, 7};
    RotationBasis basis;
    basis.dims = dims;
    basis.xPower = 1;
    basis.yPower = 1;
    basis.zPower = 1;
    
    RotationGroup * group = rotation_group_create_basis(basis);
    if (rotation_group_count(group) != 24) {
        printf("Error: <x, y, z> should have order 24, got %d.\n", 
               rotation_group_count(group));
    }
    rotation_group_release(group);
    
    basis.xPower = 2;
    basis.zPower = 2;
    group = rotation_group_create_basis(basis);
    if (rotation_group_count(group) != 8) {
        printf("Error: <x2, y, z2> should have order 8, got %d.\n",
               rotation_group_count(group));
    }
    rotation_group_release(group);
    
    basis.xPower = 0;
    basis.zPower = 0;
    group = rotation_group_create_basis(basis);
    if (rotation_group_count(group) != 4) {
        printf("Error: <y> should have order 4, got %d.\n", 
               rotation_group_count(group));
    }
    rotation_group_release(group);
    
    basis.yPower = 2;
    group = rotation_group_create_basis(basis);
    if (rotation_group_count(group) != 2) {
        printf("Error: <y2> should have order 2, got %d.\n", 
               rotation_group_count(group));
    }
    rotation_group_release(group);
    
    test_completed();
}

void test_rotation_group_cuboids() {
    
}

void test_rotation_cosets() {
    
}
