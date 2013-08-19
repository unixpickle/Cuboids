#include "representation/cuboid.h"
#include "notation/parser.h"
#include "notation/cuboid.h"
#include "arguments/search_args.h"

typedef int (* CuboidComparatorFunc)(Cuboid * cb1, Cuboid * cb2);

int cyclic_order(Cuboid * cb, CuboidComparatorFunc func);

int compare_all_but_centers(Cuboid * cb1, Cuboid * cb2);
int compare_visually(Cuboid * cb1, Cuboid * cb2);
int compare_supercube(Cuboid * cb1, Cuboid * cb2);

int main(int argc, const char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <dimensions> <algorithm>\n", argv[0]);
        return 1;
    }
    CuboidDimensions dims;
    if (!cl_sa_parse_dimensions(argv[1], &dims)) {
        fprintf(stderr, "error: failed to parse arguments.\n");
        return 1;
    }
    Algorithm * algo = algorithm_for_string(argv[2]);
    if (!algo) {
        fprintf(stderr, "error: failed to parse algorithm.\n");
        return 1;
    }
    Cuboid * transformation = algorithm_to_cuboid(algo, dims);
    algorithm_free(algo);
    if (!transformation) {
        fprintf(stderr, "error: failed to apply algorithm to cuboid.\n");
        return 1;
    }
    printf("Supercube order: %d\n", cyclic_order(transformation, compare_supercube));
    printf("Visual order: %d\n", cyclic_order(transformation, compare_visually));
    cuboid_free(transformation);
}

int cyclic_order(Cuboid * cb, CuboidComparatorFunc func) {
    int order = 0;
    Cuboid * dest = cuboid_create(cb->dimensions);
    Cuboid * identity = cuboid_create(cb->dimensions);
    do {
        cuboid_multiply_to(cb, dest);
        order++;
    } while (!func(dest, identity));
    cuboid_free(dest);
    cuboid_free(identity);
    return order;
}

int compare_all_but_centers(Cuboid * cb1, Cuboid * cb2) {
    int i;
    for (i = 0; i < cuboid_count_edges(cb1); i++) {
        CuboidEdge e1 = cb1->edges[i];
        CuboidEdge e2 = cb2->edges[i];
        if (e1.symmetry != e2.symmetry) return 0;
        if (e1.dedgeIndex != e2.dedgeIndex) return 0;
    }
    for (i = 0; i < 8; i++) {
        CuboidCorner c1 = cb1->corners[i];
        CuboidCorner c2 = cb2->corners[i];
        if (c1.symmetry != c2.symmetry) return 0;
        if (c1.index != c2.index) return 0;
    }
    return 1;
}

int compare_visually(Cuboid * cb1, Cuboid * cb2) {
    int i;
    if (!compare_all_but_centers(cb1, cb2)) return 0;
    for (i = 0; i < cuboid_count_centers(cb1); i++) {
        CuboidCenter c1 = cb1->centers[i];
        CuboidCenter c2 = cb2->centers[i];
        if (c1.side != c2.side) return 0;
    }
    return 1;
}

int compare_supercube(Cuboid * cb1, Cuboid * cb2) {
    int i;
    if (!compare_all_but_centers(cb1, cb2)) return 0;
    for (i = 0; i < cuboid_count_centers(cb1); i++) {
        CuboidCenter c1 = cb1->centers[i];
        CuboidCenter c2 = cb2->centers[i];
        if (c1.side != c2.side) return 0;
        if (c1.index != c2.index) return 0;
    }
    return 1;
}
