#include "solvers.h"
#include <stdio.h>

void printGeneralUsage(const char * command);

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        printGeneralUsage(argv[0]);
        return 1;
    }
    
    
    
    return 0;
}

void printGeneralUsage(const char * command) {
    printf("Usage: %s [<solver> <options> | resume <file.sav>]\n", command);
    puts("Options:");
    puts(" --multiple        find multiple solutions");
    puts(" --verbose         display periodic updates");
    puts(" --mindepth=n      the minimum search depth [0]");
    puts(" --maxdepth=n      the maximum search depth [20]");
    puts(" --threads=n       the number of search threads to use [8]");
    puts(" --operations <x>  the , separated operations to use");
    puts(" --dimensions <x>  the dimensions in XxYxZ format. [3x3x3]");
    printf("\n\n");
}
