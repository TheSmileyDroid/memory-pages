#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RAM_SIZE 1024
#define PAG_SIZE 32
#define NUM_PAGES (RAM_SIZE / PAG_SIZE)

struct paget {
    unsigned char referenciada : 1;
    unsigned char modificada : 1;
    unsigned char pres_aus : 1;
    unsigned char pad : 5;
    unsigned short moldura_pag;
    unsigned char mru_count;
    unsigned short ws_tempo_virtual;
};

int main() {
    struct paget page_table[NUM_PAGES] = {0};
    int next_page = 0;
    int page_faults = 0;

    srand(time(NULL));

    for (int i = 0; i < 1000000; i++) {
        int addr = rand() % RAM_SIZE;
        int page_num = addr / PAG_SIZE;

        if (!page_table[page_num].pres_aus) {
            // Page fault
            page_faults++;

            // Evict page using MRU algorithm
            int mru_page = 0;
            for (int j = 1; j < NUM_PAGES; j++) {
                if (page_table[j].pres_aus && page_table[j].mru_count > page_table[mru_page].mru_count) {
                    mru_page = j;
                }
            }

            // Replace page
            page_table[mru_page].pres_aus = 0;
            page_table[page_num].pres_aus = 1;
            page_table[page_num].moldura_pag = next_page++;
            page_table[page_num].mru_count = 0;
            page_table[page_num].ws_tempo_virtual = i;
        }

        // Update MRU count
        for (int j = 0; j < NUM_PAGES; j++) {
            if (page_table[j].pres_aus) {
                page_table[j].mru_count++;
            }
        }
        page_table[page_num].mru_count = 0;

        // Update WS timestamp
        for (int j = 0; j < NUM_PAGES; j++) {
            if (page_table[j].pres_aus) {
                page_table[j].ws_tempo_virtual = i - page_table[j].mru_count;
            }
        }
    }

    printf("Page faults: %d\n", page_faults);

    return 0;
}