#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct {
    int * sequence;
    int base, length;
} SBoundary;

typedef struct {
    // completely inclusive
    SBoundary lower;
    SBoundary upper;
} SRange;

void sboundary_initialize(SBoundary * boundary, int length, int base);
void sboundary_copy(SBoundary * boundary, SBoundary obj);
void sboundary_destroy(SBoundary boundary);

void sboundary_generate_division(SBoundary * boundary, int divisions);
void sboundary_add(SBoundary * target, SBoundary addition);
int sboundary_is_zero(SBoundary boundary);

// divisions
int srange_division(int length, int base, int divisions,
                    SRange * rangesOut);
void srange_destroy_list(SRange * ranges, int count);

int srange_minimum_digit(SRange range, int offset, const int * soFar);
int srange_maximum_digit(SRange range, int offset, const int * soFar);
