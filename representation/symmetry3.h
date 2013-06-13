/*
Symmetry map:
0 - identity
1 - (2 1 3)
2 - (1 3 2)
3 - (3 2 1)
4 - (3 1 2)
5 - (2 3 1)
*/

#include <string.h>

int symmetry3_operation_compose(int left, int right);
int symmetry3_operation_inverse(int op);
int symmetry3_operation_find(const unsigned char * orig, const unsigned char * res);
void symmetry3_operation_perform(int op, unsigned char * data);
