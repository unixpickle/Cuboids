#include "test.h"
#include "search/boundary.h"

void test_generate_division();
void test_range_division();
void test_maximum_minimum();

static int * int_list(const char * str);

int main() {
    test_generate_division();
    test_range_division();
    test_maximum_minimum();
    
    tests_completed();
    return 0;
}

void test_generate_division() {
    test_initiated("sboundary_generate_division()");
    SBoundary boundary;
    sboundary_initialize(&boundary, 5, 10);
    sboundary_generate_division(&boundary, 33);
    
    int digits1[] = {0, 3, 0, 3, 0};
    if (memcmp(boundary.sequence, digits1, sizeof(int) * 5) != 0) {
        puts("Error: failed to compute 10^5 / 33.");
    }
    
    sboundary_generate_division(&boundary, 100000 - 1);
    int digits2[] = {0, 0, 0, 0, 1};
    if (memcmp(boundary.sequence, digits2, sizeof(int) * 5) != 0) {
        puts("Error: failed to compute 10^5 / (10^5 - 1).");
    }
    
    sboundary_generate_division(&boundary, 100000);
    if (memcmp(boundary.sequence, digits2, sizeof(int) * 5) != 0) {
        puts("Error: failed to compute 10^5 / 10^5.");
    }
    
    int digits3[] = {0, 0, 0, 0, 0};
    sboundary_generate_division(&boundary, 100001);
    if (memcmp(boundary.sequence, digits3, sizeof(int) * 5) != 0) {
        puts("Error: failed to computer 10^6 / (10^6 + 1).");
    }
    
    sboundary_destroy(boundary);
    test_completed();
}

void test_range_division() {
    test_initiated("Testing srange_division()");
    SRange * ranges = (SRange *)malloc(sizeof(SRange) * 50);
    
    int count = srange_division(2, 5, 50, ranges);
    if (count != 25) {
        puts("Error: dividing 25 into 50 pieces should yield 25 pieces!");
    }
    srange_destroy_list(ranges, count);
    
    count = srange_division(2, 10, 50, ranges);
    if (count != 50) {
        puts("Error: dividing 100 by 50 should yield 50 pieces!");
    }
    
    SRange r1 = ranges[0];
    if (!sboundary_is_zero(r1.lower)) {
        puts("Error: first boundary should be zero.");
    }
    if (r1.upper.sequence[0] != 0 || r1.upper.sequence[1] != 2) {
        puts("Error: second boundary should be (0, 2).");
    }
    
    r1 = ranges[count - 1];
    if (r1.lower.sequence[0] != 9 || r1.lower.sequence[1] != 8) {
        puts("Error: invalid second to last boundary.");
    }
    if (r1.upper.sequence[0] != 10 || r1.upper.sequence[1] != 0) {
        puts("Error: invalid last boundary.");
    }
    
    srange_destroy_list(ranges, count);
    
    free(ranges);
    test_completed();
}

void test_maximum_minimum() {
    test_initiated("srange_minimum/maximum_digit()");
    
    SBoundary lower, upper;
    sboundary_initialize(&lower, 9, 12);
    sboundary_initialize(&upper, 9, 12);
    int lowerData[9] = {4, 5, 8, 6, 10, 9, 0, 0, 0};
    int upperData[9] = {10, 7, 2, 1, 3, 4, 7, 0, 0};
    memcpy(lower.sequence, lowerData, 9 * sizeof(int));
    memcpy(upper.sequence, upperData, 9 * sizeof(int));
    
    SRange range = {lower, upper};
    
    int test = srange_minimum_digit(range, 3, int_list("\x04\x05\x08"));
    if (test != 6) {
        puts("Error: minimum 3 digits failed.");
    }
    test = srange_minimum_digit(range, 4, int_list("\x05\x00\x00\x00"));
    if (test != 0) {
        puts("Error: going above minimum failed.");
    }
    test = srange_maximum_digit(range, 2, int_list("\x10\x07"));
    if (test != 2) {
        puts("Error: maximum 2 digits failed.");
    }
    test = srange_maximum_digit(range, 6, int_list("\x10\x07\x02\x01\x03\x04"));
    if (test != 6) {
        puts("Error: maximum value blocking failed.");
    }
    test = srange_maximum_digit(range, 7, int_list("\x10\x07\x02\x01\x03\x04\x06"));
    if (test != 11) {
        puts("Error: going below maximum failed.");
    }
    
    sboundary_destroy(lower);
    sboundary_destroy(upper);
    test_completed();
}

static int * int_list(const char * str) {
    static int buffer[32];
    int i;
    for (i = 0; i < strlen(str); i++) {
        buffer[i] = str[i];
    }
    return buffer;
}
