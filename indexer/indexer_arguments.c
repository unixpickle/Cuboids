#include "indexer_arguments.h"

static int _process_rotation_args(CLArgumentList * args, IndexerArguments * out);
static int _parse_rotation_axes(const unsigned char * axes, RotationBasis * basis);

CLArgumentList * indexer_default_arguments() {
    CLArgumentList * args = cl_sa_default_arguments();
    cl_argument_list_add(args, cl_argument_new_string("symmetries", ""));
}

int indexer_process_arguments(CLArgumentList * args, IndexerArguments * out) {
    if (!cl_sa_process(args, &out->search)) return 0;
    if (!_process_rotation_args(args, out)) {
        alg_list_release(out->search.operations);
        return 0;
    }
    out->symmetries.dims = out->search.dimensions;
    return 1;
}

/***********
 * Private *
 ***********/

static int _process_rotation_args(CLArgumentList * args, IndexerArguments * out) {
    RotationBasis basis = rotation_basis_standard(out->search.dimensions);
    
    index = cl_argument_list_find(args, "symmetries");
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
