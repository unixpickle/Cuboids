#include "indexer_arguments.h"

static int _process_rotation_args(CLArgumentList * args, IndexerArguments * out);
static int _process_numerical_args(CLArgumentList * args, IndexerArguments * out);
static int _process_dimensions(CLArgumentList * args, IndexerArguments * out);
static int _process_operations(CLArgumentList * args, IndexerArguments * out);
static int _parse_rotation_axes(const unsigned char * axes, RotationBasis * basis);

CLArgumentList * indexer_default_arguments() {
    CLArgumentList * args = cl_argument_list_new();
    cl_argument_list_add(args, cl_argument_new_string("symmetries", ""));
    cl_argument_list_add(args, cl_argument_new_string("operations", ""));
    cl_argument_list_add(args, cl_argument_new_string("dimensions", "3x3x3"));
    cl_argument_list_add(args, cl_argument_new_integer("threads", 1));
    cl_argument_list_add(args, cl_argument_new_integer("maxdepth", 8));
    cl_argument_list_add(args, cl_argument_new_integer("sharddepth", 3));
    return args;
}

int indexer_process_arguments(CLArgumentList * args, IndexerArguments * out) {
    if (!_process_dimensions(args, out)) return 0;
    if (!_process_rotation_args(args, out)) return 0;
    if (!_process_numerical_args(args, out)) return 0;
    if (!_process_operations(args, out)) return 0;
    return 1;
}

void indexer_remove_all_defaults(CLArgumentList * args) {
    CLArgumentList * defaults = indexer_default_arguments();
    int i;
    for (i = 0; i < cl_argument_list_count(defaults); i++) {
        int j;
        const char * name = cl_argument_list_get(defaults, i)->name;
        while ((j = cl_argument_list_find(args, name)) >= 0) {
            cl_argument_list_remove(args, j);
        }
    }
    cl_argument_list_free(defaults);
}

/***********
 * Private *
 ***********/

static int _process_rotation_args(CLArgumentList * args, IndexerArguments * out) {
    int index = cl_argument_list_find(args, "symmetries");
    RotationBasis basis = rotation_basis_standard(out->symmetries.dims);
    if (index >= 0) {
        CLArgument * arg = cl_argument_list_get(args, index);
        if (!_parse_rotation_axes(arg->contents.string.value, &out->symmetries)) {
            return 0;
        }
        if (!rotation_basis_is_subset(basis, out->symmetries)) {
            return 0;
        }
    } else {
        out->symmetries = basis;
    }
    
    return 1;
}

static int _process_numerical_args(CLArgumentList * args, IndexerArguments * out) {
    int index = cl_argument_list_find(args, "threads");
    assert(index >= 0);
    CLArgument * arg = cl_argument_list_get(args, index);
    out->threadCount = arg->contents.integer.value;
    if (out->threadCount <= 0) return 0;
    
    index = cl_argument_list_find(args, "maxdepth");
    assert(index >= 0);
    arg = cl_argument_list_get(args, index);
    out->maxDepth = arg->contents.integer.value;
    
    index = cl_argument_list_find(args, "sharddepth");
    assert(index >= 0);
    arg = cl_argument_list_get(args, index);
    out->shardDepth = arg->contents.integer.value;
    
    return 1;
}

static int _process_dimensions(CLArgumentList * args, IndexerArguments * out) {
    int index = cl_argument_list_find(args, "dimensions");
    assert(index >= 0);
    CLArgument * arg = cl_argument_list_get(args, index);
    return cl_sa_parse_dimensions(arg->contents.string.value, &out->symmetries.dims);
}

static int _process_operations(CLArgumentList * args, IndexerArguments * out) {
    int index = cl_argument_list_find(args, "operations");
    if (index < 0) {
        out->operations = cuboid_standard_basis(out->symmetries.dims);
    } else {
        CLArgument * arg = cl_argument_list_get(args, index);
        out->operations = alg_list_parse(arg->contents.string.value, 
                                         out->symmetries.dims);
    }
    return (out->operations != NULL);
}

static int _parse_rotation_axes(const unsigned char * axes, RotationBasis * basis) {
    if (strlen(axes) != 3) return 0;
    int exponents[3] = {0, 0, 0};
    int i;
    for (i = 0; i < 3; i++) {
        if (axes[i] < '0' || axes[i] > '2') return 0;
        exponents[i] = axes[i] - '0';
    }
    basis->xPower = exponents[0];
    basis->yPower = exponents[1];
    basis->zPower = exponents[2];
    return 1;
}
