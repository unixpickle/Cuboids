#include "power.h"

Cuboid * cuboid_power(Cuboid * cuboid, int power) {
    Cuboid * result = cuboid_create(cuboid->dimensions);
    int i;
    for (i = 0; i < power; i++) {
        cuboid_multiply_to(cuboid, result);
    }
    return result;
}
