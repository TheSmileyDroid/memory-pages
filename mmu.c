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

circular_list_node *clock_hand = NULL;

void relogio_insert_page_table_entry(page_table_entry *page_table_entry)
{
    circular_list_node *new_node = malloc(sizeof(circular_list_node));
    new_node->page_table_entry = page_table_entry;

    if (clock_hand == NULL)
    {
        clock_hand = new_node;
        clock_hand->next = clock_hand;
    }
    else
    {
        new_node->next = clock_hand->next;
        clock_hand->next = new_node;
    }
}

void relogio_switch_page_table_entry(page_table_entry *page_table_entry)
{
    clock_hand->page_table_entry = page_table_entry;
}

void relogio_remove_page_table_entry()
{
    circular_list_node *node = clock_hand->next;
    if (node == clock_hand)
    {
        clock_hand = NULL;
    }
    else
    {
        clock_hand->next = node->next;
    }
    free(node);
}

void relogio_print()
{
    printf("Relógio: ");
    if (clock_hand == NULL)
    {
        printf("Relógio vazio\n");
        return;
    }

    circular_list_node *node = clock_hand;
    do
    {
        printf("%d -> ", node->page_table_entry->frame);
        node = node->next;
    } while (node != clock_hand);
    printf("\n");
}

void print_page_table()
{
    printf("Página\tPresente\tQuadro\tReferenciada\tModificada\tTempo de último acesso\tContador MRU\n");
    for (int i = 0; i < NUM_PAGES; i++)
    {
        printf("%d\t%d\t\t%d\t%d\t\t%d\t\t%d\t\t\t%d\n", i, page_table[i].present, page_table[i].frame, page_table[i].referenced, page_table[i].modified, page_table[i].last_access_time, page_table[i].mru_count);
    }
}

void relogio_init()
{
    // Initialize the clock hand
    clock_hand = NULL;

    // Insert all pages in memory into the clock
    for (int i = 0; i < REAL_MEMORY_SIZE / PAGE_SIZE; i++)
    {
        relogio_insert_page_table_entry(&page_table[i]);
    }

    relogio_print();
}

void aging_init()
{
    // TODO: Implementar
}

void wsclock_init()
{
    // TODO: Implementar
}

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

    // Fill the real memory with pages
    for (int i = 0; i < REAL_MEMORY_SIZE / PAGE_SIZE; i++)
    {
        page_table[i].present = 1;
        page_table[i].frame = i;
    }

    print_page_table();

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

    // Initialize the variables for the replacement algorithms
    switch (algorithm)
    {
        case RELOGIO:
            relogio_init();
            break;
        case AGING:
            aging_init();
            break;
        case WSCLOCK:
            wsclock_init();
            break;
    }

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