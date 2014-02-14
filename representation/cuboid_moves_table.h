#ifndef __CUBOID_MOVES_TABLE_H_
#define __CUBOID_MOVES_TABLE_H_

#include <stdint.h>

// defines an X,Y,Z point on the cube
#define PT(x,y,z) ((x<<2) | (y<<1) | z)

typedef struct {
    uint8_t dedges[4];
    uint8_t dedgePieces[4];
    
    // contains a list of 1 (yes) or 0 (no) which
    // indicate whether or not to reorder the edges
    // in the destination dedge. 1 = reorder
    uint8_t dedgesReorder[4];
    
    uint8_t corners[4];
    uint8_t cornerPieces[4];
    
    uint8_t symmetry;
} FaceMap;

static const FaceMap FacesTable[] = {
    // L turn
    {
        {4, 3, 10, 9}, {9, 4, 3, 10},
        {0, 1, 0, 1},
        {PT(0, 1, 1), PT(0, 0, 1), PT(0, 0, 0), PT(0, 1, 0)},
        {PT(0, 1, 0), PT(0, 1, 1), PT(0, 0, 1), PT(0, 0, 0)},
        2
    },
    // R' turn
    {
        {5, 1, 11, 7}, {7, 5, 1, 11},
        {0, 1, 0, 1},
        {PT(1, 1, 1), PT(1, 0, 1), PT(1, 0, 0), PT(1, 1, 0)},
        {PT(1, 1, 0), PT(1, 1, 1), PT(1, 0, 1), PT(1, 0, 0)},
        2
    },
    // U turn
    {
        {6, 5, 0, 4}, {4, 6, 5, 0},
        {0, 1, 0, 1},
        {PT(0, 1, 0), PT(1, 1, 0), PT(1, 1, 1), PT(0, 1, 1)},
        {PT(0, 1, 1), PT(0, 1, 0), PT(1, 1, 0), PT(1, 1, 1)},
        3
    },
    // D' turn
    {
        {8, 11, 2, 10}, {10, 8, 11, 2},
        {0, 1, 0, 1},
        {PT(0, 0, 0), PT(1, 0, 0), PT(1, 0, 1), PT(0, 0, 1)},
        {PT(0, 0, 1), PT(0, 0, 0), PT(1, 0, 0), PT(1, 0, 1)},
        3
    },
    // F turn
    {
        {0, 1, 2, 3}, {3, 0, 1, 2},
        {1, 0, 1, 0},
        {PT(0, 1, 1), PT(1, 1, 1), PT(1, 0, 1), PT(0, 0, 1)},
        {PT(0, 0, 1), PT(0, 1, 1), PT(1, 1, 1), PT(1, 0, 1)},
        1
    },
    // B' turn
    {
        {6, 7, 8, 9}, {9, 6, 7, 8},
        {1, 0, 1, 0},
        {PT(0, 1, 0), PT(1, 1, 0), PT(1, 0, 0), PT(0, 0, 0)},
        {PT(0, 0, 0), PT(0, 1, 0), PT(1, 1, 0), PT(1, 0, 0)},
        1
    }
};

typedef struct {
    uint8_t dedges[8];
    uint8_t centers[8];
    uint8_t symmetry;
    
    // the line information is used to read centers.
    // this line information should allow the program to read
    // each center in a line in the same direction (ordering) as
    // the centers of the slice are ordered.
    
    uint8_t lineAxis[4]; // 0 = x, 1 = y
    uint8_t flipHoriz[4];
    uint8_t flipVert[4];
} SliceMap;

static const SliceMap SlicesTable[] = {
    {{0, 2, 8, 6},
     {1, 4, 2, 3},
     2,
     {1, 1, 1, 1},
     {0, 0, 1, 0},
     {0, 0, 1, 0}
    },
    {{1, 3, 9, 7},
     {1, 6, 2, 5},
     3,
     {0, 0, 0, 0},
     {1, 1, 1, 1},
     {0, 0, 0, 0}
    },
    {{4, 5, 11, 10},
     {3, 5, 4, 6},
     1,
     {0, 1, 0, 1},
     {0, 0, 1, 1},
     {1, 0, 0, 1}}
};

#endif