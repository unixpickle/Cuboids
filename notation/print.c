#include "print.h"

static void _algorithm_print_full(int isRoot, const Algorithm * a, FILE * fp);
static void _algorithm_print_body(int isRoot, const Algorithm * a, FILE * fp);

void algorithm_print(const Algorithm * a, FILE * fp) {
    _algorithm_print_full(1, a, fp);
}

static void _algorithm_print_full(int isRoot, const Algorithm * a, FILE * fp) {
    _algorithm_print_body(isRoot, a, fp);
    if (a->power != 1) {
        fprintf(fp, "%d", a->power);
    }
    if (a->inverseFlag) fprintf(fp, "'");
}

static void _algorithm_print_body(int isRoot, const Algorithm * a, FILE * fp) {
    if (a->type == AlgorithmTypeWideTurn) {
        if (a->contents.wideTurn.numLayers == 1) {
            fprintf(fp, "%c", a->contents.wideTurn.face);
        } else {
            fprintf(fp, "%d%c%c", a->contents.wideTurn.numLayers,
                    a->contents.wideTurn.face, 'w');
        }
    } else if (a->type == AlgorithmTypeSlice) {
        fprintf(fp, "%c", a->contents.slice.layer);
    } else if (a->type == AlgorithmTypeRotation) {
        fprintf(fp, "%c", a->contents.rotation.axis);
    } else {
        if (!isRoot) fprintf(fp, "(");
        int i;
        for (i = 0; i < algorithm_container_count(a); i++) {
            if (i != 0) {
                fprintf(fp, " ");
            }
            
            Algorithm * subAlgo = algorithm_container_get(a, i);
            _algorithm_print_full(0, subAlgo, fp);
        }
        if (!isRoot) fprintf(fp, ")");
    }
}
