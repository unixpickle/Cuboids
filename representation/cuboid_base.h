#ifndef __CUBOID_BASE_H__
#define __CUBOID_BASE_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "symmetry3.h"

/*

IMPORTANT STANDARDS:
- the faces are indexed 1-6:
    1 front, 2 back, 3 top, 4 bottom, 5 right, 6 left
- the dedges are indexed 0-11:
    0 front top, 1 front right, 2 front bottom, 3 front left
    4 top left, 5 top right
    6 back top, 7 back right, 8 back bottom, 9 back left
    10 bottom left, 11 bottom right
- the edges are indexed 0-(n - 3)
    on the M slice, they go from left to right
    on the E slice, they go from top down,
    on the S slice, they go from front to back
- the corners are indexed using 3 tuple coordinates:
    the tuple (x,y,z) is encoded as an integer by masking
    x = 4, y = 2, z = 1; for example, the coordinates (1,0,1) = 101b, (1,0,0)=100b
    the origin is the bottom back left, x is horizontal, y vertical, z forward/backward
- the centers are addressed by a face and an index:
    the index of a center piece is found by reading the centers from
    left to right, top to bottom, starting with 0
    to orient the cube for reading off centers, hold the 3 face on top
    except for entering the 3 face where you hold the 2 face on top,
    or entering the 4 face where you hold the 1 face on top

*/

typedef struct {
#if __BIG_ENDIAN
    unsigned short edgeIndex : 8; // index of the edge in the dedge
    unsigned short symmetry : 4; // the symmetry of the edge
    unsigned short dedgeIndex : 4; // the dedge index
#else
    unsigned short dedgeIndex : 4; // the dedge index
    unsigned short symmetry : 4; // the symmetry of the edge
    unsigned short edgeIndex : 8; // index of the edge in the dedge
#endif
} __attribute__((__packed__)) CuboidEdge;

typedef struct {
#if __BIG_ENDIAN
    unsigned char symmetry : 4;
    unsigned char index : 4;
#else
    unsigned char index : 4;
    unsigned char symmetry : 4;
#endif
} __attribute__((__packed__)) CuboidCorner;

typedef struct {
    uint8_t side; // the side 1-6
    uint16_t index; // index in the center;
    // the index field may be ignored if an insufficient
    // amount of information was available to figure it out
} __attribute__((__packed__)) CuboidCenter;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t padding;
} __attribute__((__packed__)) CuboidDimensions;

// The cuboid structure is a representation of the physical structure
// of an XxYxZ cuboid. The pointers in the structure represent physical slots.
// The values in these pointers represent physical *pieces* to fill the slots.

// For example, on a 3x3x3 cuboid, the front center would always be located at index
// [0] in the centers pointer. That is, after an M slice, the center at index
// [0] will be 3 rather than 1.

typedef struct {
    // a face is encoded at a time, so the centers for face 1 will come first,
    // then the centers for face 2, and so on
    CuboidCenter * centers; // or NULL
    
    // the edges are encoded from dedge 0 to 11, and for each one
    // the edges are encoded in order.
    CuboidEdge * edges; // or NULL
    
    // this ordering is straightforward if you see the standard above
    CuboidCorner * corners;
    
    CuboidDimensions dimensions;
} Cuboid;

int cuboid_dimensions_equal(CuboidDimensions d1, CuboidDimensions d2);

Cuboid * cuboid_create(CuboidDimensions dimensions);
void cuboid_free(Cuboid * cuboid);

void cuboid_multiply(Cuboid * out, const Cuboid * left, const Cuboid * right);
void cuboid_multiply_to(const Cuboid * left, Cuboid * right);
Cuboid * cuboid_copy(const Cuboid * cuboid);
void cuboid_copy_to(Cuboid * copy, const Cuboid * cuboid);

uint16_t cuboid_edge_index(const Cuboid * cuboid, int dedge, int edge);
uint16_t cuboid_center_index(const Cuboid * cuboid, int face, int index);

uint8_t cuboid_count_edges_for_dedge(const Cuboid * cuboid, int dedge);
uint16_t cuboid_count_edges(const Cuboid * cuboid);

uint16_t cuboid_count_centers_for_face(const Cuboid * cuboid, int number);
uint16_t cuboid_count_centers(const Cuboid * cuboid);

#endif
