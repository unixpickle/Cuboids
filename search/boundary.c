#include "boundary.h"

void sboundary_initialize(SBoundary * boundary, int length, int base) {
    boundary->length = length;
    boundary->base = base;
    boundary->sequence = (int *)malloc(sizeof(int) * length);
    bzero(boundary->sequence, sizeof(int) * length);
}

void sboundary_copy(SBoundary * boundary, SBoundary obj) {
    sboundary_initialize(boundary, obj.length, obj.base);
    memcpy(boundary->sequence, obj.sequence,
           sizeof(int) * obj.length);
}

void sboundary_destroy(SBoundary boundary) {
    free(boundary.sequence);
}

/********************
 * Boundary methods *
 ********************/

void sboundary_generate_division(SBoundary * boundary, int divisions) {
    int i, theRemainder = 0;
    boundary->sequence[0] = boundary->base / divisions;
    theRemainder = boundary->base % divisions;
    for (i = 1; i < boundary->length && theRemainder != 0; i++) {
        int nextTotal = theRemainder * boundary->base;
        boundary->sequence[i] = nextTotal / divisions;
        theRemainder = nextTotal % divisions;
    }
}

void sboundary_add(SBoundary * target, SBoundary addition) {
    assert(target->length == addition.length);
    assert(target->base == addition.base);
    int i;
    int carry = 0;
    for (i = target->length - 1; i >= 0; i--) {
        target->sequence[i] += carry;
        target->sequence[i] += addition.sequence[i];
        if (target->sequence[i] >= addition.base) {
            target->sequence[i] -= addition.base;
            carry = 1;
        } else carry = 0;
    }
}

int sboundary_is_zero(SBoundary boundary) {
    int i;
    for (i = 0; i < boundary.length; i++) {
        if (boundary.sequence[i] != 0) return 0;
    }
    return 1;
}

/*******************
 * Range divisions *
 *******************/

int srange_division(int length, int base, int divisions,
                    SRange * rangesOut) {
    // create divisions
    SBoundary step, currentMin;
    int i;
    sboundary_initialize(&step, length, base);
    sboundary_initialize(&currentMin, length, base);
    sboundary_generate_division(&step, divisions);
    
    if (sboundary_is_zero(step)) {
        divisions = 1; // set divisions = base^length
        for (i = 0; i < length; i++) divisions *= base;
        sboundary_generate_division(&step, divisions);
    }
    
    for (i = 0; i < divisions; i++) {
        SRange range;
        sboundary_copy(&range.lower, currentMin);
        if (i + 1 < divisions) {
            sboundary_add(&currentMin, step);
            sboundary_copy(&range.upper, currentMin);
        } else {
            SBoundary endBound;
            sboundary_initialize(&endBound, length, base);
            endBound.sequence[0] = base;
            range.upper = endBound;
        }
        rangesOut[i] = range;
    }
    
    sboundary_destroy(step);
    sboundary_destroy(currentMin);
    return divisions;
}

void srange_destroy_list(SRange * ranges, int count) {
    int i;
    for (i = 0; i < count; i++) {
        sboundary_destroy(ranges[i].lower);
        sboundary_destroy(ranges[i].upper);
    }
}

int srange_minimum_digit(SRange range, int offset, const int * soFar) {
    int i, touchingLowerBound = 1;
    for (i = 0; i < offset; i++) {
        if (soFar[i] > range.lower.sequence[i]) {
            touchingLowerBound = 0;
            break;
        }
    }
    if (!touchingLowerBound) return 0;
    return range.lower.sequence[offset];
}

int srange_maximum_digit(SRange range, int offset, const int * soFar) {
    // we must be *less* than the upper bound
    int i, isLastUpper = 1, touchingUpperBound = 1;
    for (i = offset + 1; i < range.upper.length; i++) {
        if (range.upper.sequence[i] != 0) {
            isLastUpper = 0;
            break;
        }
    }
    for (i = 0; i < offset; i++) {
        if (soFar[i] < range.upper.sequence[i]) {
            touchingUpperBound = 0;
            break;
        }
    }
    if (!touchingUpperBound) return range.upper.base - 1;
    if (isLastUpper) {
        return range.upper.sequence[offset] - 1;
    } else {
        return range.upper.sequence[offset];
    }
}
