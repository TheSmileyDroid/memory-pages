#include <stdio.h>

#define RAM_SIZE 1024 * 1024 * 16 // 16 MB
#define PAG_SIZE 4096

struct paget
{
    unsigned char referenciada : 1;
    unsigned char modificada : 1;
    unsigned char pres_aus : 1;
    unsigned char pad : 5;
    unsigned short moldura_pag;
    unsigned char mru_count;
    unsigned short ws_tempo_virtual;
    // ...
};

int main()
{
    int num_pages = RAM_SIZE / PAG_SIZE;
    struct paget page_table[num_pages];
    int next_page = 0;

    // Access memory randomly
    for (int i = 0; i < 1000000; i++)
    {
        int addr = rand() % RAM_SIZE;
        int page_num = addr / PAG_SIZE;
        if (!page_table[page_num].pres_aus)
        {
            // Page fault
            page_table[page_num].pres_aus = 1;
            page_table[page_num].moldura_pag = next_page++;
            if (next_page == num_pages)
            {
                next_page = 0;
            }
        }
        page_table[page_num].referenciada = 1;
    }

    return 0;
}