#include "stickers/mapconversion.h"
#include "representation/cuboid_htmoves.h"
#include "test.h"

void test_3x3x3_from_stickers();
void test_back_and_forth();

int main() {
    test_3x3x3_from_stickers();
    test_back_and_forth();
    
    tests_completed();
    return 0;
}

void test_3x3x3_from_stickers() {
    test_initiated("sticker map -> 3x3x3");
    
    CuboidDimensions dim = {3, 3, 3};
    
    // scrambled with D L2 D2 R2 B2 D' F2 R' F2 L' R' U' R2 U2 L2 U2 B D' U2 L' D2 B U' (23 HTM)
    const char * stickerData = "321312451" "166126216" "445436546" "524542453" "313653536" "212365142";
    StickerMap * stickers = stickermap_create(dim);
    int i;
    for (i = 0; i < 9 * 6; i++) {
        stickers->stickers[i] = stickerData[i] - '1' + 1;
    }
    Cuboid * cuboid = cuboid_create(dim);
    if (!convert_sm_to_cb(cuboid, stickers)) {
        puts("Error: conversion returned failure.");
    }
    
    // Yes, I did manually enter this. No, I did not enjoy it.
    uint8_t corners[] = {1, 4, 0, 6, 2, 5, 7, 3};
    uint8_t edges[] = {8, 9, 7, 5, 2, 3, 10, 0, 1, 4, 11, 6};
    for (i = 0; i < 8; i++) {
        CuboidCorner corner = cuboid->corners[i];
        if (corner.index != corners[i]) {
            printf("Error: invalid corner at index %d, found %d.\n", i, corner.index);
        }
    }
    
    for (i = 0; i < 12; i++) {
        CuboidEdge edge = cuboid->edges[i];
        if (edge.dedgeIndex != edges[i]) {
            printf("Error: invalid edge at index %d, found %d.\n", i, edge.dedgeIndex);
        }
    }
    
    stickermap_free(stickers);
    cuboid_free(cuboid);
    
    test_completed();
}

void test_back_and_forth() {
    test_initiated("conversion back and forth");
    
    CuboidDimensions dim = {3, 4, 3};
    const char * stickerData = "655434665456" "433333556643" "222122221"
                               "111111221" "365546363364" "534645454645";
    StickerMap * stickers = stickermap_create(dim);
    int i;
    for (i = 0; i < 12 * 4 + 9 * 2; i++) {
        stickers->stickers[i] = stickerData[i] - '1' + 1;
    }
    Cuboid * cuboid = cuboid_create(dim);
    convert_sm_to_cb(cuboid, stickers);
    convert_cb_to_sm(stickers, cuboid);
    for (i = 0; i < 12 * 4 + 9 * 2; i++) {
        if (stickers->stickers[i] != stickerData[i] - '1' + 1) {
            printf("Error: sticker at index %d has miraculously changed!\n", i);
        }
    }
    
    stickermap_free(stickers);
    cuboid_free(cuboid);
    
    test_completed();
}
