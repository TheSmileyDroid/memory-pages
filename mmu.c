#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MR_SIZE 10
#define MV_SIZE 20
#define MS_SIZE 10

struct page {
    unsigned char present : 1;
    unsigned char pad : 5;
    unsigned short frame;
    unsigned char mru_count;
    unsigned short last_access_time;
};

struct page MR[MR_SIZE]; // real memory
struct page MV[MV_SIZE]; // virtual memory
struct page MS[MS_SIZE]; // swap memory

int page_miss_count = 0;

// Function to generate a random page number
int generate_random_page() {
    return rand() % MV_SIZE;
}

// Function to handle page faults
void handle_page_fault(int page_number) {
    // Find a free frame in MR
    int free_frame = -1;
    for (int i = 0; i < MR_SIZE; i++) {
        if (MR[i].present == 0) {
            free_frame = i;
            break;
        }
    }
    // If no free frame is found, select a victim page to swap out
    if (free_frame == -1) {
        page_miss_count++;
        int victim_page = -1;
        for (int i = 0; i < MR_SIZE; i++) {
            if (MR[i].mru_count == 0) {
                victim_page = i;
                break;
            }
        }
        if (victim_page == -1) {
            victim_page = rand() % MR_SIZE;
        }
        // Swap out the victim page to MS
        MS[MR[victim_page].frame] = MR[victim_page];
        MR[victim_page].present = 0;
        // Find a free frame in MR
        for (int i = 0; i < MR_SIZE; i++) {
            if (MR[i].present == 0) {
                free_frame = i;
                break;
            }
        }
    }
    // Swap in the requested page to MR
    MR[free_frame] = MV[page_number];
    MR[free_frame].present = 1;
    MR[free_frame].frame = free_frame;
    MR[free_frame].mru_count = MR_SIZE;
    MR[free_frame].last_access_time = time(NULL);
    // Update MRU counts for all pages in MR
    for (int i = 0; i < MR_SIZE; i++) {
        if (MR[i].present == 1) {
            MR[i].mru_count--;
        }
    }
}

int main() {
    // Initialize random number generator
    srand(time(NULL));
    // Simulate page references
    for (int i = 0; i < 100; i++) {
        int page_number = generate_random_page();
        if (MV[page_number].present == 0) {
            handle_page_fault(page_number);
        } else {
            MR[MV[page_number].frame].mru_count = MR_SIZE;
            MR[MV[page_number].frame].last_access_time = time(NULL);
        }
    }
    // print results
    printf("Page miss count: %d\n", page_miss_count);

    return 0;
}