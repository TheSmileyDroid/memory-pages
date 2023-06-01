#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Simulador de MMU (Memory Management Unit)
/**
 * Teste das implementações de Relógio, Envelhecimento e WSClock
 */

#define VIRTUAL_ADDRESS_SPACE_SIZE 65536
#define PAGE_SIZE 4096
#define NUM_PAGES 16
#define REAL_MEMORY_SIZE 32768

typedef struct
{
    unsigned char referenced : 1;
    unsigned char modified : 1;
    unsigned char present : 1;
    unsigned char pad : 5;
    unsigned short frame;
    unsigned char mru_count;
    unsigned short last_access_time;
} page_table_entry;

page_table_entry page_table[NUM_PAGES];

unsigned int page_miss_count = 0;
unsigned int complexidade = 0;

typedef enum {RELOGIO, AGING, WSCLOCK} page_replacement_algorithm;

page_replacement_algorithm algorithm = RELOGIO;

typedef struct circular_list_node
{
    page_table_entry *page_table_entry;
    struct circular_list_node *next;
} circular_list_node;

// Page replacement algorithms
unsigned int relogio()
{
    printf("Relógio\n");
    // Implementar o algoritmo de relógio

    return -1;
}

unsigned int aging()
{
    printf("Envelhecimento\n");
    // Implementar o algoritmo de envelhecimento

    return -1;
}

unsigned int wsclock()
{
    printf("WSClock\n");
    // Implementar o algoritmo de WSClock

    return -1;
}

unsigned int virtual_to_physical(unsigned int virtual_address)
{
    unsigned int page_index = virtual_address / PAGE_SIZE;
    unsigned int page_offset = virtual_address % PAGE_SIZE;

    if (page_index >= NUM_PAGES || !page_table[page_index].present)
    {
        // Page miss
        page_miss_count++;

        // Memory is always full, so we need to replace a page

        // Call the page replacement algorithm
        unsigned int frame = 0;
        switch (algorithm)
        {
            case RELOGIO:
                frame = relogio();
                break;
            case AGING:
                frame = aging();
                break;
            case WSCLOCK:
                frame = wsclock();
                break;
        }

        // Load the page into memory
        page_table[page_index].frame = frame;

        // Update the page table
        page_table[page_index].present = 1;

        // Return the physical address
        unsigned int physical_address = frame * PAGE_SIZE + page_offset;

        return physical_address;
    }

    unsigned int physical_address = page_table[page_index].frame * PAGE_SIZE + page_offset;
    return physical_address;
}

void clock_tick()
{
    // Implementar o clock tick
} 

int main(int argc, char **argv)
{
    for (int i = 0; i < NUM_PAGES; i++)
    {
        page_table[i].present = 0;
        page_table[i].frame = -1;
    }

    // Select a page replacement algorithm
    if (argc > 1)
    {
        if (argv[1][0] == 'c')
        {
            algorithm = RELOGIO;
        }
        else if (argv[1][0] == 'a')
        {
            algorithm = AGING;
        }
        else if (argv[1][0] == 'w')
        {
            algorithm = WSCLOCK;
        }
        else {
            printf("Invalid algorithm\n");
            return 1;
        }
    }

    // Try to access a page that is present in memory
    unsigned int virtual_address = 0x1234;
    page_table[1].present = 1;
    page_table[1].frame = 2;
    unsigned int physical_address = virtual_to_physical(virtual_address);
    printf("Virtual address: %x\n", virtual_address);
    printf("Physical address: %x\n", physical_address);

    // Fill the real memory with pages
    for (int i = 0; i < REAL_MEMORY_SIZE / PAGE_SIZE; i++)
    {
        page_table[i].present = 1;
        page_table[i].frame = i;
    }

    // Try to access a page that is present in memory
    virtual_address = 0x1234;
    physical_address = virtual_to_physical(virtual_address);
    printf("Virtual address: %x\n", virtual_address);
    printf("Physical address: %x\n", physical_address);

    // Try to access a page that is not present in memory
    virtual_address = 0x9123;
    physical_address = virtual_to_physical(virtual_address);
    printf("Virtual address: %x\n", virtual_address);
    printf("Physical address: %x\n", physical_address);

    /**
     * Ideia dos testes:
     * Calcular a complexidade dos algoritmos toda vez que ele tenha que tomar alguma decisão
     * Calcular a quantidade de Page Miss
     *
     * Testar com diferentes tempos de acesso
     * Testar com diferentes desvios nos valores aleatórios
     * Testar com diferentes Algoritmos de Substituição de Páginas
     *
     *
     * Para a entrada, usar um arquivo de texto com os endereços virtuais a serem acessados,
     * feitos em outra linguagem que possua uma função aleatória de distribuição normal
     *
     * Para as interrupções e para os tempos usar o i da iteração do loop
     * Para cada loop realizar X acessos a memória
     * e a cada X acessos, realizar uma interrupção
     *
     * Para encontrar o tau, checar o ultimo tick, para variar o tau dinamicamente
     */

    return 0;
}