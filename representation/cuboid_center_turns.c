#import "cuboid_center_turns.h"

int qt_center_point_to_index(int size, int x, int y) {
    y *= -1;
    x += size / 2;
    y += size / 2;
    
    if (size % 2 == 0) {
        if (x >= size / 2) x--;
        if (y >= size / 2) y--;
    }
    
    return (y * size) + x;
}

void qt_center_point_from_index(int size, int index, int * x, int * y) {
    *y = index / size;
    *x = index % size;
    
    if (size % 2 == 0) {
        if (*y >= size / 2) *y += 1;
        if (*x >= size / 2) *x += 1;
    }
    
    *y -= size / 2;
    *x -= size / 2;
    *y *= -1;
}

void qt_rotate_center_point(int face, int * x, int * y) {
    // this is very, very, very basic trig.
    int clockwise = 1, tmpX = *x, tmpY = *y;
    if (face == 2 || face == 5 || face == 4) clockwise = 0;
    if (clockwise) {
        *x = tmpY;
        *y = -tmpX;
    } else {
        *x = -tmpY;
        *y = tmpX;
    }
}
