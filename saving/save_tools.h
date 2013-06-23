#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void save_uint8(uint8_t number, FILE * fp);
void save_uint32(uint32_t number, FILE * fp);
void save_uint16(uint16_t number, FILE * fp);
void save_uint64(uint64_t number, FILE * fp);
void save_string(const char * str, FILE * fp);

int load_uint8(uint8_t * out, FILE * fp);
int load_uint32(uint32_t * out, FILE * fp);
int load_uint16(uint16_t * out, FILE * fp);
int load_uint64(uint64_t * out, FILE * fp);
char * load_string(FILE * fp);
