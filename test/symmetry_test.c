#include "algebra/rotation_cosets.h"
#include "notation/parser.h"
#include "test.h"

void test_rotation_group_counts();
void test_rotation_group_cuboids();
void test_rotation_cosets();

int main(int argc, const char * argv[]) {
    test_rotation_group_counts();
    test_rotation_group_cuboids();
    test_rotation_cosets();
    
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
    test_initiated("a common rotation group");
    
    CuboidDimensions dims = {3, 3, 3};
    RotationBasis basis = {dims, 2, 1, 2};
    RotationGroup * group = rotation_group_create_basis(basis);
    
    Algorithm * algo = algorithm_for_string("x2 y");
    Cuboid * shouldHave = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    algo = algorithm_for_string("x y2");
    Cuboid * shouldLack = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    
    if (!rotation_group_contains(group, shouldHave)) {
        puts("Error: <x2, y, z2> should contain (x2 y).");
    }
    if (rotation_group_contains(group, shouldLack)) {
        puts("Error: <x2, y, z2> should not contain (x y2).");
    }
    
    cuboid_free(shouldHave);
    cuboid_free(shouldLack);
    rotation_group_release(group);
    test_completed();
}

void test_rotation_cosets() {
    test_initiated("rotation cosets");
    
    CuboidDimensions dims = {3, 3, 3};
    RotationBasis basis = {dims, 2, 1, 2};
    RotationBasis standard = rotation_basis_standard(dims);
    
    RotationGroup * mainG = rotation_group_create_basis(standard);
    RotationGroup * subG = rotation_group_create_basis(basis);
    RotationCosets * cosets = rotation_cosets_create(mainG, subG);
    if (rotation_cosets_count(cosets) != 3) {
        printf("Error: <x,y,z>\\<x2,y,z2> should have order 3, got %d.\n",
               rotation_cosets_count(cosets));
    }
    
    rotation_cosets_release(cosets);
    rotation_group_release(mainG);
    rotation_group_release(subG);
    
    test_completed();
}
