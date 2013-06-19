#include "save_tools.h"

void save_uint32(uint32_t number, FILE * fp) {
    uint8_t bytes[] = {number & 0xff, (number >> 8) & 0xff,
                       (number >> 16) & 0xff, (number >> 24) & 0xff};
    fwrite(bytes, 1, 4, fp);
}

void save_uint16(uint16_t number, FILE * fp) {
    uint8_t bytes[] = {number & 0xff, (number >> 8) & 0xff};
    fwrite(bytes, 1, 2, fp);
}

int load_uint32(uint32_t * out, FILE * fp) {
    uint8_t bytes[4];
    if (fread(bytes, 1, 4, fp) != 4) return 0;
    *out = (bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24));
    return 1;
}

int load_uint16(uint16_t * out, FILE * fp) {
    uint8_t bytes[2];
    if (fread(bytes, 1, 2, fp) != 4) return 0;
    *out = (bytes[0] | (bytes[1] << 8));
    return 1;
}
