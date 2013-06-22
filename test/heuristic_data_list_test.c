#include "heuristic/data_list.h"
#include "test.h"

void test_full_sharded();
void test_half_sharded();
void test_no_sharded();

void test_data_list(DataList * list);
uint8_t make_checksum(const uint8_t * ptr, int len);

int main(int argc, const char * argv[]) {
    test_full_sharded();
    test_half_sharded();
    test_no_sharded();
    
    tests_completed();
    return 0;
}

void test_full_sharded() {
    test_initiated("fully sharded data_list");
    
    DataList * list = data_list_create(4, 1, 4);
    test_data_list(list);
    data_list_free(list);
    
    test_completed();
}

void test_half_sharded() {
    test_initiated("half sharded data_list");
    
    DataList * list = data_list_create(4, 1, 2);
    test_data_list(list);
    data_list_free(list);
    
    test_completed();
}

void test_no_sharded() {
    test_initiated("non-sharded data_list");
    
    DataList * list = data_list_create(4, 1, 0);
    test_data_list(list);
    data_list_free(list);
    
    test_completed();
}

void test_data_list(DataList * list) {
    uint8_t counters[4] = {0, 0, 0, 0};
    uint8_t zeros[4] = {0, 0, 0, 0};
    int isDone = 0;
    while (!isDone) {
        uint8_t header = make_checksum(counters, 4);
        DataListNode * node = data_list_find_base(list, counters, 1);
        assert(node != NULL);
        data_list_base_add(node, counters, &header);
        
        // increment counters
        int i;
        for (i = 0; i < 4; i++) {
            if (counters[i] == 0x20) {
                counters[i] = 0;
                if (i == 3) isDone = 1;
            } else {
                counters[i]++;
                if (i == 3) {
                    printf("added %u of 32...\r", counters[3]);
                    fflush(stdout);
                }
                break;
            }
        }
    }
    printf("\n");
    assert(memcmp(counters, zeros, 4) == 0);
    isDone = 0;
    while (!isDone) {
        uint8_t expectedHeader = make_checksum(counters, 4);
        uint8_t * header;
        DataListNode * base = data_list_find_base(list, counters, 0);
        if (!base) {
            printf("\nError: no base found for %d %d %d %d.\n",
                   counters[0], counters[1], counters[2], counters[3]);
            return;
        }
        int found = data_list_base_find(base, counters, &header);
        if (!found) {
            printf("\nError: no entry found for %d %d %d %d.\n",
                   counters[0], counters[1], counters[2], counters[3]);
            return;
        }
        if (*header != expectedHeader) {
            printf("\nError: expected %d but got %d for %d %d %d %d.\n",
                   expectedHeader, *header,
                   counters[0], counters[1], counters[2], counters[3]);
            return;
        }
        
        int i;
        for (i = 0; i < 4; i++) {
            if (counters[i] == 0x20) {
                counters[i] = 0;
                if (i == 3) isDone = 1;
            } else {
                counters[i]++;
                if (i == 3) {
                    printf("tested %u of 32...\r", counters[3]);
                    fflush(stdout);
                }
                break;
            }
        }
    }
    printf("\n");
}

uint8_t make_checksum(const uint8_t * ptr, int len) {
    uint8_t chk = 0xc4;
    int i;
    for (i = 0; i < len; i++) {
        chk ^= ptr[i];
    }
    return chk;
}
