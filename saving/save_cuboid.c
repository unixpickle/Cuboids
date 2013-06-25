#include "save_cuboid.h"

static void _save_cuboid_edges(const Cuboid * cuboid, FILE * fp);
static void _save_cuboid_centers(const Cuboid * cuboid, FILE * fp);
static void _save_cuboid_corners(const Cuboid * cuboid, FILE * fp);

static int _load_into_cuboid(Cuboid * cuboid, FILE * fp);
static int _load_cuboid_edges(Cuboid * cuboid, FILE * fp);
static int _load_cuboid_centers(Cuboid * cuboid, FILE * fp);
static int _load_cuboid_corners(Cuboid * cuboid, FILE * fp);


void save_cuboid(const Cuboid * cuboid, FILE * fp) {
    fprintf(fp, "<CUBOID");
    save_cuboid_dimensions(cuboid->dimensions, fp);
    _save_cuboid_edges(cuboid, fp);
    _save_cuboid_centers(cuboid, fp);
    _save_cuboid_corners(cuboid, fp);
    fprintf(fp, "CUBOID>");
}

Cuboid * load_cuboid(FILE * fp) {
    char buffer[8];
    fread(buffer, 1, 7, fp);
    if (memcmp(buffer, "<CUBOID", 7) != 0) return NULL;
    
    CuboidDimensions dims;
    if (!load_cuboid_dimensions(&dims, fp)) return NULL;
    Cuboid * cuboid = cuboid_create(dims);
    
    if (!_load_into_cuboid(cuboid, fp)) {
        cuboid_free(cuboid);
        return NULL;
    }
    
    fread(buffer, 1, 7, fp);
    if (memcmp(buffer, "CUBOID>", 7) != 0) {
        cuboid_free(cuboid);
        return NULL;
    }
    
    return cuboid;
}

void save_cuboid_dimensions(CuboidDimensions dims, FILE * fp) {
    uint8_t x = dims.x, y = dims.y, z = dims.z;
    fwrite(&x, 1, 1, fp);
    fwrite(&y, 1, 1, fp);
    fwrite(&z, 1, 1, fp);
}

int load_cuboid_dimensions(CuboidDimensions * dims, FILE * fp) {
    uint8_t x, y, z;
    if (fread(&x, 1, 1, fp) != 1) return 0;
    if (fread(&y, 1, 1, fp) != 1) return 0;
    if (fread(&z, 1, 1, fp) != 1) return 0;
    dims->x = x;
    dims->y = y;
    dims->z = z;
    return 1;
}

/******************
 * Private Saving *
 ******************/

static void _save_cuboid_edges(const Cuboid * cuboid, FILE * fp) {
    int count = cuboid_count_edges(cuboid);
    int i;
    for (i = 0; i < count; i++) {
        CuboidEdge edge = cuboid->edges[i];
        uint8_t dedgeIndex = edge.dedgeIndex;
        uint8_t edgeIndex = edge.edgeIndex;
        uint8_t symmetry = edge.symmetry;
        fwrite(&dedgeIndex, 1, 1, fp);
        fwrite(&edgeIndex, 1, 1, fp);
        fwrite(&symmetry, 1, 1, fp);
    }
}

static void _save_cuboid_centers(const Cuboid * cuboid, FILE * fp) {
    int count = cuboid_count_centers(cuboid);
    int i;
    for (i = 0; i < count; i++) {
        CuboidCenter c = cuboid->centers[i];
        uint8_t side = c.side;
        uint8_t index = c.index;
        fwrite(&side, 1, 1, fp);
        fwrite(&index, 1, 1, fp);
    }
}

static void _save_cuboid_corners(const Cuboid * cuboid, FILE * fp) {
    int i;
    for (i = 0; i < 8; i++) {
        CuboidCorner c = cuboid->corners[i];
        uint8_t index = c.index;
        uint8_t symmetry = c.symmetry;
        fwrite(&index, 1, 1, fp);
        fwrite(&symmetry, 1, 1, fp);
    }
}

/*******************
 * Private Loading *
 *******************/

static int _load_into_cuboid(Cuboid * cuboid, FILE * fp) {
    if (!_load_cuboid_edges(cuboid, fp)) return 0;
    if (!_load_cuboid_centers(cuboid, fp)) return 0;
    if (!_load_cuboid_corners(cuboid, fp)) return 0;
    return 1;
}

static int _load_cuboid_edges(Cuboid * cuboid, FILE * fp) {
    int edgeCount = cuboid_count_edges(cuboid);
    int i;
    for (i = 0; i < edgeCount; i++) {
        CuboidEdge edge;
        uint8_t dedgeIndex;
        uint8_t edgeIndex;
        uint8_t symmetry;
        if (fread(&dedgeIndex, 1, 1, fp) != 1) return 0;
        if (fread(&edgeIndex, 1, 1, fp) != 1) return 0;
        if (fread(&symmetry, 1, 1, fp) != 1) return 0;
        edge.dedgeIndex = dedgeIndex;
        edge.edgeIndex = edgeIndex;
        edge.symmetry = symmetry;
        cuboid->edges[i] = edge;
    }
    return 1;
}

static int _load_cuboid_centers(Cuboid * cuboid, FILE * fp) {
    int centerCount = cuboid_count_centers(cuboid);
    int i;
    for (i = 0; i < centerCount; i++) {
        CuboidCenter c;
        uint8_t side;
        uint8_t index;
        if (fread(&side, 1, 1, fp) != 1) return 0;
        if (fread(&index, 1, 1, fp) != 1) return 0;
        c.side = side;
        c.index = index;
        cuboid->centers[i] = c;
    }
    return 1;
}

static int _load_cuboid_corners(Cuboid * cuboid, FILE * fp) {
    int i;
    for (i = 0; i < 8; i++) {
        CuboidCorner corner;
        uint8_t index;
        uint8_t symmetry;
        if (fread(&index, 1, 1, fp) != 1) return 0;
        if (fread(&symmetry, 1, 1, fp) != 1) return 0;
        corner.index = index;
        corner.symmetry = symmetry;
        cuboid->corners[i] = corner;
    }
    return 1;
}
