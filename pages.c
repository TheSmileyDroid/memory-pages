#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MR_SIZE 100 // size of real memory
#define MV_SIZE 1000 // size of virtual memory
#define MS_SIZE (MV_SIZE - MR_SIZE) // size of swap memory

struct page {
    unsigned char referenced : 1;
    unsigned char modified : 1;
    unsigned char present : 1;
    unsigned char pad : 5;
    unsigned short frame;
    unsigned char mru_count;
    unsigned short last_access_time;
};

struct page MR[MR_SIZE]; // real memory
struct page MV[MV_SIZE]; // virtual memory
struct page MS[MS_SIZE]; // swap memory

int page_fault_count = 0;


int main() {
    

    // print results
    printf("Page fault count: %d\n", page_fault_count);

    return 0;
}