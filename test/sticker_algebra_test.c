#include "algebra/sticker_algebra.h"
#include "representation/cuboid_htmoves.h"
#include "stickers/mapconversion.h"
#include "test.h"

void test_dedge_paired();
void test_center_solved();
void test_cube_solved();

int main() {
    test_dedge_paired();
    test_center_solved();
    test_cube_solved();
    
    tests_completed();
    return 0;
}

void test_dedge_paired() {
    test_initiated("cuboid_is_dedge_paired() on a 4x4x4");
    
    CuboidDimensions dim = {4, 4, 4};
    Cuboid * lowerE = cuboid_half_slice(dim, CuboidMovesAxisY, 1);
    int i;
    for (i = 0; i < 12; i++) {
        int isPaired = cuboid_is_dedge_paired(lowerE, i);
        int expected = (i != 1 && i != 3 && i != 7 && i != 9);
        if (isPaired != expected) {
            printf("Error: invalid response for dedge %d.\n", i);
        }
    }
    
    cuboid_free(lowerE);
    
    test_completed();
}

void test_center_solved() {
    test_initiated("cuboid_is_center_solid() on an 11x11x11");
    
    CuboidDimensions dim = {11, 11, 11};
    Cuboid * slice = cuboid_half_slice(dim, CuboidMovesAxisZ, 3);
    int i;
    for (i = 1; i <= 6; i++) {
        int isSolid = cuboid_is_center_solid(slice, i);
        int expected = i == 1 || i == 2;
        if (expected != isSolid) {
            printf("Error: invalid response for face %d.\n", i);
        }
    }
    
    cuboid_free(slice);
    test_completed();
}

void test_cube_solved() {
    test_initiated("stickermap_face_is_solid_color() on 4x4x4");
    
    CuboidDimensions dim = {4, 4, 4};
    Cuboid * mSlice = cuboid_half_slice(dim, CuboidMovesAxisX, 1);
    Cuboid * eSlice = cuboid_half_slice(dim, CuboidMovesAxisY, 0);
    Cuboid * cuboid = cuboid_create(dim);
    
    int i;
    for (i = 0; i < 4; i++) {
        cuboid_multiply_to(mSlice, cuboid);
        cuboid_multiply_to(eSlice, cuboid);
    }
    
    StickerMap * map = stickermap_create(dim);
    convert_cb_to_sm(map, cuboid);
    
    uint8_t expected[] = {0, 0, 1, 1, 1, 1};
    for (i = 1; i <= 6; i++) {
        int isSolved = stickermap_face_is_solid_color(map, i);
        if (isSolved != expected[i - 1]) {
            printf("Error: invalid response for face %d.\n", i);
        }
    }
    
    stickermap_free(map);
    cuboid_free(mSlice);
    cuboid_free(eSlice);
    cuboid_free(cuboid);
    
    test_completed();
}
