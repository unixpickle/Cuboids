#include "cuboid_base.h"

static void _initialize_cuboid_edges(Cuboid * cuboid);
static void _initialize_add_edges(Cuboid * cuboid, int offset, int dedge);

static void _initialize_cuboid_corners(Cuboid * cuboid);
static void _initialize_cuboid_centers(Cuboid * cuboid);

static void _multiply_corners(Cuboid * out, const Cuboid * left, const Cuboid * right);
static void _multiply_edges(Cuboid * out, const Cuboid * left, const Cuboid * right);
static void _multiply_centers(Cuboid * out, const Cuboid * left, const Cuboid * right);

int cuboid_dimensions_equal(CuboidDimensions d1, CuboidDimensions d2) {
    if (d1.x != d2.x) return 0;
    if (d1.y != d2.y) return 0;
    if (d1.z != d2.z) return 0;
    return 1;
}

Cuboid * cuboid_create(CuboidDimensions dimensions) {
    Cuboid * cuboid = (Cuboid *)malloc(sizeof(Cuboid));
    bzero(cuboid, sizeof(Cuboid));
    cuboid->dimensions = dimensions;
    
    _initialize_cuboid_corners(cuboid);
    
    if (dimensions.x >= 3 || dimensions.y >= 3 || dimensions.z >= 3) {
        // there are edges
        _initialize_cuboid_edges(cuboid);
    }
    
    if (cuboid_count_centers(cuboid) > 0) {
        _initialize_cuboid_centers(cuboid);
    }
    
    return cuboid;
}

void cuboid_free(Cuboid * cuboid) {
    if (cuboid->corners) {
        free(cuboid->corners);
    }
    if (cuboid->edges) {
        free(cuboid->edges);
    }
    if (cuboid->centers) {
        free(cuboid->centers);
    }
    free(cuboid);
}

void cuboid_multiply(Cuboid * out, const Cuboid * left, const Cuboid * right) {
    assert(cuboid_dimensions_equal(left->dimensions, right->dimensions));
    assert(cuboid_dimensions_equal(left->dimensions, out->dimensions));
    _multiply_corners(out, left, right);
    if (cuboid_count_edges(left) > 0) {
        _multiply_edges(out, left, right);
    }
    if (cuboid_count_centers(left) > 0) {
        _multiply_centers(out, left, right);
    }
}

void cuboid_multiply_to(const Cuboid * left, Cuboid * right) {
    Cuboid * temp = cuboid_copy(right);
    cuboid_multiply(right, left, temp);
    cuboid_free(temp);
}

Cuboid * cuboid_copy(const Cuboid * cuboid) {
    Cuboid * copy = (Cuboid *)malloc(sizeof(Cuboid));
    bzero(copy, sizeof(Cuboid));
    copy->dimensions = cuboid->dimensions;
    int centerCount = cuboid_count_centers(copy);
    int edgeCount = cuboid_count_edges(copy);
    
    copy->corners = (CuboidCorner *)malloc(sizeof(CuboidCorner) * 8);
    memcpy(copy->corners, cuboid->corners,
           sizeof(CuboidCorner) * 8);
          
    if (edgeCount > 0) {
        copy->edges = (CuboidEdge *)malloc(sizeof(CuboidEdge) * edgeCount);
        memcpy(copy->edges, cuboid->edges,
               sizeof(CuboidEdge) * edgeCount);
    }
    
    if (centerCount > 0) {
        copy->centers = (CuboidCenter *)malloc(sizeof(CuboidCenter) * centerCount);
        memcpy(copy->centers, cuboid->centers,
               sizeof(CuboidCenter) * centerCount);
    }
    
    return copy;
}

void cuboid_copy_to(Cuboid * copy, const Cuboid * cuboid) {
    assert(cuboid_dimensions_equal(copy->dimensions, cuboid->dimensions));
    int centerCount = cuboid_count_centers(copy);
    int edgeCount = cuboid_count_edges(copy);
    
    memcpy(copy->corners, cuboid->corners,
           sizeof(CuboidCorner) * 8);
          
    if (edgeCount > 0) {
        memcpy(copy->edges, cuboid->edges,
               sizeof(CuboidEdge) * edgeCount);
    }
    
    if (centerCount > 0) {
        memcpy(copy->centers, cuboid->centers,
               sizeof(CuboidCenter) * centerCount);
    }
}

/**************
 * Addressing *
 **************/

uint16_t cuboid_edge_index(const Cuboid * cuboid, int dedge, int edge) {
    // optimized for cubic cuboids
    if (cuboid->dimensions.x == cuboid->dimensions.y &&
        cuboid->dimensions.y == cuboid->dimensions.z) {
        int edgeSize = cuboid->dimensions.x - 2;
        return dedge * edgeSize + edge;
    }
    
    int absEdgeIndex = 0, i;
    for (i = 0; i < dedge; i++) {
        absEdgeIndex += cuboid_count_edges_for_dedge(cuboid, i);
    }
    absEdgeIndex += edge;
    return absEdgeIndex;
}

uint16_t cuboid_center_index(const Cuboid * cuboid, int face, int index) {
    // optimized for cubic cuboids
    if (cuboid->dimensions.x == cuboid->dimensions.y &&
        cuboid->dimensions.y == cuboid->dimensions.z) {
        int centerSize = (cuboid->dimensions.x - 2) * (cuboid->dimensions.x - 2);
        return (face - 1) * centerSize + index;
    }
    
    int absCenterIndex = 0, i;
    for (i = 1; i < face; i++) {
        absCenterIndex += cuboid_count_centers_for_face(cuboid, i);
    }
    absCenterIndex += index;
    return absCenterIndex;
}

/***************************************
 * General cuboid-specific information *
 ***************************************/

// edges

uint8_t cuboid_count_edges_for_dedge(const Cuboid * cuboid, int dedge) {
    // 0 = x, 1 = y, 2 = z
    const int dedgeAxes[] = {0, 1, 0, 1, 2, 2, 0, 1, 0, 1, 2, 2};
    int counts[] = {cuboid->dimensions.x - 2, cuboid->dimensions.y - 2,
                    cuboid->dimensions.z - 2};
    return counts[dedgeAxes[dedge]];
}

uint16_t cuboid_count_edges(const Cuboid * cuboid) {
    CuboidDimensions dim = cuboid->dimensions;
    return (dim.x - 2) * 4 + (dim.y - 2) * 4 + (dim.z - 2) * 4;
}

// centers

uint16_t cuboid_count_centers_for_face(const Cuboid * cuboid, int number) {
    uint16_t xdim = cuboid->dimensions.x - 2;
    uint16_t ydim = cuboid->dimensions.y - 2;
    uint16_t zdim = cuboid->dimensions.z - 2;
    
    if (number == 1 || number == 2) {
        return xdim * ydim;
    }
    if (number == 3 || number == 4) {
        return xdim * zdim;
    }
    return ydim * zdim;
}

uint16_t cuboid_count_centers(const Cuboid * cuboid) {
    uint16_t xdim = cuboid->dimensions.x - 2;
    uint16_t ydim = cuboid->dimensions.y - 2;
    uint16_t zdim = cuboid->dimensions.z - 2;
    return xdim * ydim * 2 + ydim * zdim * 2 + xdim * zdim * 2;
}

/***********
 * PRIVATE *
 ***********/

// initialization

static void _initialize_cuboid_edges(Cuboid * cuboid) {
    int edgeSize = sizeof(CuboidEdge);
    int edgeCount = cuboid_count_edges(cuboid);
    cuboid->edges = (void *)malloc(edgeSize * edgeCount);
    
    int i, completed = 0;
    for (i = 0; i < 12; i++) {
        int numEdges = cuboid_count_edges_for_dedge(cuboid, i);
        _initialize_add_edges(cuboid, completed, i);
        completed += numEdges;
    }
}

static void _initialize_add_edges(Cuboid * cuboid, int offset, int dedge) {
    int count = cuboid_count_edges_for_dedge(cuboid, dedge);
    int i, current = offset;
    for (i = 0; i < count; i++) {
        CuboidEdge edge;
        edge.edgeIndex = i;
        edge.dedgeIndex = dedge;
        edge.symmetry = 0;
        cuboid->edges[current] = edge;
        current++;
    }
}

static void _initialize_cuboid_corners(Cuboid * cuboid) {
    cuboid->corners = (CuboidCorner *)malloc(sizeof(CuboidCorner) * 8);
    int i;
    for (i = 0; i < 8; i++) {
        CuboidCorner corner;
        corner.symmetry = 0;
        corner.index = i;
        cuboid->corners[i] = corner;
    }
}

static void _initialize_cuboid_centers(Cuboid * cuboid) {
    int centerSize = sizeof(CuboidCenter);
    int centerCount = cuboid_count_centers(cuboid);
    cuboid->centers = (CuboidCenter *)malloc(centerSize * centerCount);
    
    int i, j, offset = 0;
    for (i = 1; i <= 6; i++) {
        int numCenters = cuboid_count_centers_for_face(cuboid, i);
        for (j = 0; j < numCenters; j++) {
            CuboidCenter c;
            c.side = i;
            c.index = j;
            cuboid->centers[offset] = c;
            offset++;
        }
    }
}

// multiplication

static void _multiply_corners(Cuboid * out, const Cuboid * left, const Cuboid * right) {
    int i;
    for (i = 0; i < 8; i++) {
        CuboidCorner leftCorner = left->corners[i];
        int rightIndex = leftCorner.index;
        CuboidCorner rightCorner = right->corners[rightIndex];
        CuboidCorner outCorner = rightCorner;
        outCorner.symmetry = symmetry3_operation_compose(leftCorner.symmetry,
                                                        rightCorner.symmetry);
        out->corners[i] = outCorner;
    }
}

static void _multiply_edges(Cuboid * out, const Cuboid * left, const Cuboid * right) {
    int i, edgeCount = cuboid_count_edges(left);
    for (i = 0; i < edgeCount; i++) {
        CuboidEdge leftEdge = left->edges[i];
        int rightIndex = cuboid_edge_index(right,
                                           leftEdge.dedgeIndex,
                                           leftEdge.edgeIndex);
        CuboidEdge rightEdge = right->edges[rightIndex];
        CuboidEdge outEdge = rightEdge;
        outEdge.symmetry = symmetry3_operation_compose(leftEdge.symmetry,
                                                      rightEdge.symmetry);
        out->edges[i] = outEdge;
    }
}

static void _multiply_centers(Cuboid * out, const Cuboid * left, const Cuboid * right) {
    // this is the simple composition operation on a permutation ;)
    int i, centerCount = cuboid_count_centers(left);
    for (i = 0; i < centerCount; i++) {
        CuboidCenter leftCenter = left->centers[i];
                
        int rightIndex = cuboid_center_index(right,
                                             leftCenter.side,
                                             leftCenter.index);
        out->centers[i] = right->centers[rightIndex];
    }
}
