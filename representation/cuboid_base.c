#include "cuboid_base.h"

static void _initialize_cuboid_edges(Cuboid * cuboid);
static void _initialize_add_edges(Cuboid * cuboid, int offset, int dedge, int extended);

static void _initialize_cuboid_corners(Cuboid * cuboid);
static void _initialize_cuboid_centers(Cuboid * cuboid);

static void _multiply_corners(Cuboid * out, const Cuboid * left, const Cuboid * right);
static void _multiply_edges(Cuboid * out, const Cuboid * left, const Cuboid * right);
static void _multiply_basic_edges(Cuboid * out, const Cuboid * left, const Cuboid * right);

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
}

void cuboid_multiply(Cuboid * out, const Cuboid * left, const Cuboid * right) {
    assert(cuboid_dimensions_equal(left.dimensions, right.dimensions));
    _multiply_corners(out, left, right);
}

Cuboid * cuboid_copy(Cuboid * cuboid) {
    
}

/**************
 * Addressing *
 **************/

void * cuboid_edge_address(Cuboid * cuboid, int dedge, int edge) {
    int absEdgeIndex = 0, i;
    for (i = 0; i < dedge; i++) {
        absEdgeIndex += cuboid_count_edges_for_dedge(cuboid, i);
    }
    absEdgeIndex += edge;
    int edgeSize = cuboid_edge_data_size(cuboid);
    return &cuboid->edges[edgeSize * absEdgeIndex];
}

void * cuboid_center_address(Cuboid * center, int face, int index) {
    int absCenterIndex = 0, i;
    for (i = 1; i < face; i++) {
        absCenterIndex += cuboid_count_centers_for_face(cuboid, i);
    }
    absCenterIndex += index;
    int centerSize = sizeof(CuboidCenter);
    return &cuboid->centers[centerSize * absCenterIndex];
}

/***************************************
 * General cuboid-specific information *
 ***************************************/

// edges

uint8_t cuboid_count_edges_for_dedge(Cuboid * cuboid, int dedge) {
    // 0 = x, 1 = y, 2 = z
    const int dedgeAxes[] = {0, 1, 0, 1, 2, 2, 0, 1, 0, 1, 2, 2};
    int counts[] = {cuboid->dimensions.x - 2, cuboid->dimensions.y - 2,
                    cuboid->dimensions.z - 2};
    return counts[dedgeAxes[dedge]];
}

uint16_t cuboid_count_edges(Cuboid * cuboid) {
    dim = cuboid->dimensions;
    return (dim.x - 2) * 4 + (dim.y - 2) * 4 + (dim.z - 2) * 4;
}

size_t cuboid_edge_data_size(Cuboid * cuboid) {
    int extended = cuboid_uses_extended_edges(cuboid);
    if (extended) {
        return sizeof(CuboidEdge);
    } else {
        return sizeof(CuboidBasicEdge);
    }
}

int cuboid_uses_extended_edges(Cuboid * cuboid) {
    int extendDedges = 0;
    CuboidDimensions dim = cuboid->dimensions;
    if (dim.x > 3 || dim.y > 3 || dim.z > 3) {
        extendDedges = 1;
    }
    return extendDedges;
}

// centers

uint16_t cuboid_count_centers_for_face(Cuboid * cuboid, int number) {
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

uint16_t cuboid_count_centers(Cuboid * cuboid) {
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
    int extendedFlag = cuboid_uses_extended_dedges(cuboid);
    int edgeSize = cuboid_edge_data_size(cuboid);
    int edgeCount = cuboid_count_edges(cuboid);
    cuboid->edges = (void *)malloc(edgeSize * edgeCount);
    
    int i, completed = 0;
    for (i = 0; i < 12; i++) {
        int numEdges = cuboid_count_edges_for_dedge(cuboid, i);
        _initialize_add_edges(cuboid, completed, i, extendedFlag);
        completed += numEdges;
    }
}

static void _initialize_add_edges(Cuboid * cuboid, int offset, int dedge, int extended) {
    void * buffer = &cuboid->edges[offset * cuboid_edge_data_size(cuboid)];
    int count = cube_count_edges_for_dedge(cuboid, dedge);
    int i;
    for (i = 0; i < count; i++) {
        if (extended) {
            CuboidEdge edge;
            edge.edgeIndex = i;
            edge.dedgeIndex = dedge;
            edge.symmetry = 0;
            memcpy(buffer, &edge, sizeof(edge));
            buffer = &buffer[sizeof(edge)];
        } else {
            CuboidBasicEdge edge;
            edge.dedgeIndex = i;
            edge.symmetry = 0;
            memcpy(buffer, &edge, sizeof(edge));
            buffer = &buffer[sizeof(edge)];
        }
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
        int numCenters = cuboid_count_centers_for_face(cuboid);
        for (j = 0; j < numCenters; j++) {
            void * dest = &cuboid->centers[offset * centerSize];
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
        CuboidCorner outCorner;
        outCorner.symmetry = symmetry_operation_compose(leftCorner.symmetry,
                                                        rightCorner.symmetry);
        outCorner.index = rightCorner.index;
        out.corners[i] = outCorner;
    }
}

static void _multiply_edges(Cuboid * out, const Cuboid * left, const Cuboid * right) {
    if (!cuboid_uses_extended_edges(left)) {
        _multiply_basic_edges(out, left, right);
        return;
    } 
    int edgeCount = cuboid_count_edges(left);
    int i;
    for (i = 0; i < edgeCount; i++) {
        CuboidEdge * leftEdge = &left->edges[sizeof(CuboidEdge) * i];
        CuboidEdge * rightEdge = 
    }
}

static void _multiply_basic_edges(Cuboid * out, const Cuboid * left, const Cuboid * right) {
    
}
