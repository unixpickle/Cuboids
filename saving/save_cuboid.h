#include "representation/cuboid_base.h"
#include <stdio.h>

void save_cuboid(const Cuboid * cuboid, FILE * fp);
Cuboid * load_cuboid(FILE * fp);
