#include "inverse.h"

static void _cuboid_inverse_edges(Cuboid * inverse, const Cuboid * cuboid);
static void _cuboid_inverse_corners(Cuboid * inverse, const Cuboid * cuboid);
static void _cuboid_inverse_centers(Cuboid * inverse, const Cuboid * cuboid);

Cuboid * cuboid_inverse(Cuboid * cuboid) {
    Cuboid * inverse = cuboid_create(cuboid->dimensions);
    _cuboid_inverse_edges(inverse, cuboid);
    _cuboid_inverse_corners(inverse, cuboid);
    _cuboid_inverse_centers(inverse, cuboid);
    return inverse;
}

static void _cuboid_inverse_edges(Cuboid * inverse, const Cuboid * cuboid) {
    int i, j;
    for (i = 0; i < 12; i++) {
        int dedgeCount = cuboid_count_edges_for_dedge(cuboid, i);
        for (j = 0; j < dedgeCount; j++) {
            int sourceIndex = cuboid_edge_index(cuboid, i, j);
            CuboidEdge edge = cuboid->edges[sourceIndex];
            int destIndex = cuboid_edge_index(inverse, edge.dedgeIndex,
                                              edge.edgeIndex);
            CuboidEdge dest;
            dest.symmetry = symmetry3_operation_inverse(edge.symmetry);
            dest.edgeIndex = j;
            dest.dedgeIndex = i;
            inverse->edges[destIndex] = dest;
        }
    }
}

static void _cuboid_inverse_corners(Cuboid * inverse, const Cuboid * cuboid) {
    int i;
    for (i = 0; i < 8; i++) {
        CuboidCorner c = cuboid->corners[i];
        CuboidCorner newC;
        newC.symmetry = symmetry3_operation_inverse(c.symmetry);
        newC.index = i;
        inverse->corners[c.index] = newC;
    }
}

static void _cuboid_inverse_centers(Cuboid * inverse, const Cuboid * cuboid) {
    int i, j;
    for (i = 1; i <= 6; i++) {
        int centerCount = cuboid_count_centers_for_face(cuboid, i);
        for (j = 0; j < centerCount; j++) {
            int sourceIndex = cuboid_center_index(cuboid, i, j);
            CuboidCenter c = cuboid->centers[sourceIndex];
            int destIndex = cuboid_center_index(inverse, c.side, c.index);
            CuboidCenter newC;
            newC.side = i;
            newC.index = j;
            inverse->centers[destIndex] = newC;
        }
    }
}
