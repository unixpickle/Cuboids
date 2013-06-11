#include "symmetry3.h"

int symmetry3_operation_compose(int left, int right) {
    const int table[] = {0, 1, 2, 3, 4, 5,
                         1, 0, 4, 5, 2, 3,
                         2, 5, 0, 4, 3, 1,
                         3, 4, 5, 0, 1, 2,
                         4, 3, 1, 2, 5, 0,
                         5, 2, 3, 1, 0, 4};
    return table[6 * left + right];
}

int symmetry3_operation_inverse(int op) {
    const int table[] = {0, 1, 2, 3, 5, 4};
    return table[op];
}

int symmetry3_operation_find(const unsigned char * orig, const unsigned char * res) {
    // check if all the same elements are there
    int foundElements = 0;
    int i;
    for (i = 0; i < 3; i++) {
        int j;
        for (j = 0; j < 3; j++) {
            if (res[j] == orig[i]) {
                foundElements ++;
                break;
            }
        }
    }
    if (foundElements != 3) return -1;
    if (memcmp(orig, res, 3) == 0) return 0;
    if (res[0] == orig[0]) return 2;
    if (res[2] == orig[2]) return 1;
    if (res[1] == orig[1]) return 3;
    if (res[0] == orig[2]) return 4;
    if (res[2] == orig[0]) return 5;
    return -1;
}

void symmetry3_operation_perform(int op, unsigned char * data) {
    if (op == 0) return;
    unsigned char el1 = data[0];
    unsigned char el2 = data[1];
    unsigned char el3 = data[2];
    switch (op) {
        case 1:
            data[1] = el1;
            data[0] = el2;
            break;
        case 2:
            data[1] = el3;
            data[2] = el2;
            break;
        case 3:
            data[0] = el3;
            data[2] = el1;
            break;
        case 4:
            data[0] = el3;
            data[1] = el1;
            data[2] = el2;
            break;
        case 5:
            data[2] = el1;
            data[0] = el2;
            data[1] = el3;
            break;
        default:
            break;
    }
}
