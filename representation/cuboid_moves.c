#include "cuboid_moves.h"

FaceMap cuboid_moves_face_map(CuboidMovesAxis axis, int offset) {
    assert(offset == -1 || offset == 1);
    int indexes[3][2] = {{0, 1}, {3, 2}, {5, 4}};
    int index = indexes[axis][(offset + 1) / 2];
    return FacesTable[index];
}

SliceMap cuboid_moves_slice_map(CuboidMovesAxis axis) {
    return SlicesTable[axis];
}

int cuboid_face_for_face_turn(CuboidMovesAxis axis, int offset) {
    assert(offset == -1 || offset == 1);
    int indexes[3][2] = {{6, 5}, {4, 3}, {2, 1}};
    return indexes[axis][(offset + 1) / 2];
}

int cuboid_slice_center_line_length(CuboidDimensions dim,
                                    SliceMap sliceMap,
                                    int centerIndex) {
    int w, h;
    cuboid_center_dimensions(dim, sliceMap.centers[centerIndex], &w, &h);
    if (sliceMap.lineAxis[centerIndex] == 0) {
        return w;
    } else return h;
}

int cuboid_slice_center_line_index(CuboidDimensions dim,
                                   SliceMap sliceMap,
                                   int centerIndex, // 0-3
                                   int lineIndex,
                                   int indexInLine) {
    int w, h;
    cuboid_center_dimensions(dim, sliceMap.centers[centerIndex], &w, &h);
    
    int xValue = 0, yValue = 0;
    if (sliceMap.lineAxis[centerIndex] == 0) {
        // the line is horizontal
        if (sliceMap.flipVert[centerIndex]) {
            yValue = h - lineIndex - 1;
        } else yValue = lineIndex;
        if (sliceMap.flipHoriz[centerIndex]) {
            xValue = w - indexInLine - 1;
        } else xValue = indexInLine;
    } else {
        // the line is vertical
        if (sliceMap.flipHoriz[centerIndex]) {
            xValue = w - lineIndex - 1;
        } else xValue = lineIndex;
        if (sliceMap.flipVert[centerIndex]) {
            yValue = h - indexInLine - 1;
        } else yValue = indexInLine;
    }
    return (yValue * w) + xValue;
}
