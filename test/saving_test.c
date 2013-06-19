#include "saving/save_cuboid.h"
#include "notation/cuboid.h"
#include "notation/parser.h"
#include "test.h"

void test_save_cuboid();

int main() {
    test_save_cuboid();
    
    tests_completed();
    return 0;
}

void test_save_cuboid() {
    test_initiated("save_cuboid");
    
    CuboidDimensions dims = {10, 20, 10};
    Algorithm * algo = algorithm_for_string("R2 2Bw2 3Dw L2 5Bw2 F2 4Rw2 L2 Fw2 2Uw'");
    Cuboid * cuboid = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    
    FILE * temp = tmpfile();
    assert(temp != NULL);
    save_cuboid(cuboid, temp);
    fseek(temp, 0, SEEK_SET);
    Cuboid * cb = load_cuboid(temp);
    fclose(temp);
    
    if (!cb) {
        puts("Error: failed to load cuboid at all!");
    }
    
    if (!cuboid_dimensions_equal(cb->dimensions, cuboid->dimensions)) {
        puts("Error: the dimensions do not match.");
    }
    
    int i, j;
    for (i = 0; i < 12; i++) {
        int edgeCount = cuboid_count_edges_for_dedge(cuboid, i);
        for (j = 0; j < edgeCount; j++) {
            int index = cuboid_edge_index(cuboid, i, j);
            CuboidEdge e1 = cuboid->edges[index];
            CuboidEdge e2 = cb->edges[index];
            if (e1.symmetry != e2.symmetry || e1.dedgeIndex != e2.dedgeIndex
                || e1.edgeIndex != e2.edgeIndex) {
                    printf("Error: edge mismatch at (%d, %d).\n", i, j);
            }
        }
    }
    for (i = 1; i <= 6; i++) {
        int count = cuboid_count_centers_for_face(cuboid, i);
        for (j = 0; j < count; j++) {
            int index = cuboid_center_index(cuboid, i, j);
            CuboidCenter c1 = cuboid->centers[index];
            CuboidCenter c2 = cb->centers[index];
            if (c1.side != c2.side || c1.index != c2.index) {
                printf("Error: center mismatch at (%d, %d).\n", i, j);
            }
        }
    }
    for (i = 0; i < 8; i++) {
        CuboidCorner c1 = cuboid->corners[i];
        CuboidCorner c2 = cb->corners[i];
        if (c1.index != c2.index || c1.symmetry != c2.symmetry) {
            printf("Error: corner mismatch at %d.\n", i);
        }
    }
    
    cuboid_free(cb);
    cuboid_free(cuboid);
    test_completed();
}
