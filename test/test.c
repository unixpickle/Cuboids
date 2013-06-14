#include "test.h"

void test_initiated(const char * info) {
    printf("Testing %s...\n", info);
}

void test_completed() {
    puts("Test complete.");
}

void tests_completed() {
    puts("Hit return to exit...");
    char buf[3];
    fgets(buf, 3, stdin);
}