#ifndef __MAPCONVERSION_TABLE_H__
#define __MAPCONVERSION_TABLE_H__

#include <stdint.h>

typedef struct {
    struct {
        uint8_t face;
        uint8_t position; // 0 = top, 1 = right, 2 = bottom, 3 = left
        uint8_t flipFlag;
    } sides[2];
} DedgeMap;

const static DedgeMap DedgesTable[] = {
    {{ // dedge 0
        {3, 2, 0}, // UF, y
        {1, 0, 0} // FU, z
    }},
    {{ // dedge 1
        {5, 3, 0}, // RF, x
        {1, 1, 0} // FR, z
    }},
    {{ // dedge 2
        {4, 0, 0}, // DF, y
        {1, 2, 0} // FD, z
    }},
    {{ // dedge 3
        {6, 1, 0}, // LF, x
        {1, 3, 0} // FL, z
    }},
    {{ // dedge 4
        {6, 0, 1}, // LU, x
        {3, 3, 1} // UL, y
    }},
    {{ // dedge 5
        {5, 0, 0}, // RU, x
        {3, 1, 1} // UR, y
    }},
    {{ // dedge 6
        {3, 0, 0}, // UB, y
        {2, 0, 1} // BU, z
    }},
    {{ // dedge 7
        {5, 1, 0}, // RB, x
        {2, 3, 0} // BR, z
    }},
    {{ // dedge 8
        {4, 2, 0}, // DB, y
        {2, 2, 1} // BD, z
    }},
    {{ // dedge 9
        {6, 3, 0}, // LB, x
        {2, 1, 0} // BL, z
    }},
    {{ // dedge 10
        {6, 2, 1}, // LD, x
        {4, 3, 0} // DL, y
    }},
    {{ // dedge 11
        {5, 2, 0}, // RD, x
        {4, 1, 0} // DR, y
    }}
};

typedef struct {
    struct {
        int face;
        uint8_t x; // 1 or 0
        uint8_t y; // 1 or 0
    } sides[3];
} CornerMap;

const static CornerMap CornersTable[] = {
    {{{6, 0, 1}, {4, 0, 1}, {2, 1, 1}}}, // (0, 0, 0)
    {{{6, 1, 1}, {4, 0, 0}, {1, 0, 1}}}, // (0, 0, 1)
    {{{6, 0, 0}, {3, 0, 0}, {2, 1, 0}}}, // (0, 1, 0)
    {{{6, 1, 0}, {3, 0, 1}, {1, 0, 0}}}, // (0, 1, 1)
    {{{5, 1, 1}, {4, 1, 1}, {2, 0, 1}}}, // (1, 0, 0)
    {{{5, 0, 1}, {4, 1, 0}, {1, 1, 1}}}, // (1, 0, 1)
    {{{5, 1, 0}, {3, 1, 0}, {2, 0, 0}}}, // (1, 1, 0)
    {{{5, 0, 0}, {3, 1, 1}, {1, 1, 0}}} // (1, 1, 1)
};

const static uint8_t CornerPieces[8][3] = {
    {6,4,2}, // 000
    {6,4,1}, // 001
    {6,3,2}, // 010
    {6,3,1}, // 011
    {5,4,2}, // 100
    {5,4,1}, // 101
    {5,3,2}, // 110
    {5,3,1}  // 111
};

const static uint8_t EdgePieces[12][3] = {
    {0,3,1},
    {5,0,1},
    {0,4,1},
    {6,0,1},
    {6,3,0},
    {5,3,0},
    {0,3,2},
    {5,0,2},
    {0,4,2},
    {6,0,2},
    {6,4,0},
    {5,4,0}
};

#endif