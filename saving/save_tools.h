#include <stdio.h>
#include <stdint.h>

void save_uint32(uint32_t number, FILE * fp);
void save_uint16(uint16_t number, FILE * fp);
void save_uint64(uint64_t number, FILE * fp);

int load_uint32(uint32_t * out, FILE * fp);
int load_uint16(uint16_t * out, FILE * fp);
int load_uint64(uint64_t * out, FILE * fp);
