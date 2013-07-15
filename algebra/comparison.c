#include "comparison.h"

int cuboid_light_comparison(const Cuboid * c1, const Cuboid * c2) {
    int i;
    int edgeCount = cuboid_count_edges(c1);
    int centerCount = cuboid_count_centers(c1);
    for (i = 0; i < edgeCount; i++) {
        CuboidEdge e1 = c1->edges[i];
        CuboidEdge e2 = c2->edges[i];
        if (e1.dedgeIndex < e2.dedgeIndex) return -1;
        if (e1.dedgeIndex > e2.dedgeIndex) return 1;
        if (e1.symmetry < e2.symmetry) return -1;
        if (e1.symmetry > e2.symmetry) return 1;
    }
    for (i = 0; i < centerCount; i++) {
        CuboidCenter ce1 = c1->centers[i];
        CuboidCenter ce2 = c2->centers[i];
        if (ce1.side < ce2.side) return -1;
        if (ce1.side > ce2.side) return 1;
    }
    for (i = 0; i < 8; i++) {
        CuboidCorner co1 = c1->corners[i];
        CuboidCorner co2 = c2->corners[i];
        if (co1.index < co2.index) return -1;
        if (co1.index > co2.index) return 1;
        if (co1.symmetry < co2.symmetry) return -1;
        if (co1.symmetry > co2.symmetry) return 1;
    }
    return 0;
}