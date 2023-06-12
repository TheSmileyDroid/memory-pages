#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define true 1
#define false 0

// Simulador de MMU (Memory Management Unit)
/**
 * Teste das implementações de Relógio, Envelhecimento e WSClock
 */

#define VIRTUAL_ADDRESS_SPACE_SIZE 65536 // Memory size of 64KB
#define PAGE_SIZE 4096                   // Page size of 4KB
#define NUM_PAGES 16                     // Number of pages in the virtual address space
#define REAL_MEMORY_SIZE 32768           // Memory size of 32KB

#define UNKNOW_PAGE 65535

typedef struct page_table_entry
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

unsigned long ticks = 0;
unsigned long page_miss_count = 0;
unsigned long page_acess_count = 0;
unsigned long test_aux_1 = 0;
unsigned long test_aux_2 = 0;
unsigned long complexidade = 0;

typedef enum
{
    RELOGIO,
    AGING,
    WSCLOCK
} page_replacement_algorithm;

page_replacement_algorithm algorithm = RELOGIO;

typedef struct circular_list_node
{
    unsigned int virtual_page;
    page_table_entry *page_table_entry;
    struct circular_list_node *next;
} circular_list_node;

circular_list_node *clock_hand = NULL;

void relogio_insert_page_table_entry(unsigned int virtual_page)
{

    page_table_entry *page_table_entry = &page_table[virtual_page];

    circular_list_node *new_node = malloc(sizeof(circular_list_node));
    new_node->page_table_entry = page_table_entry;
    new_node->virtual_page = virtual_page;

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

void relogio_free()
{
    if (clock_hand == NULL)
    {
        return;
    }

    // Traverse the list and free each node
    circular_list_node *current = clock_hand->next;
    while (current != clock_hand)
    {
        circular_list_node *next = current->next;
        free(current);
        current = next;
    }

    // Free the last node and reset the clock hand
    free(clock_hand);
    clock_hand = NULL;
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
        printf("%d -> ", node->virtual_page);
        node = node->next;
    } while (node != clock_hand);
    printf("\n");
}

void print_page_table()
{
    printf("Página\tPresente\tQuadro\tReferenciada\tModificada\tTempo de último acesso\tContador MRU\n");
    for (int i = 0; i < NUM_PAGES; i++)
    {
        printf("%u\t\t%u\t\t%u\t\t%u\t\t\t%u\t\t%u\t\t\t\t%u\n", i, page_table[i].present, page_table[i].frame, page_table[i].referenced, page_table[i].modified, page_table[i].last_access_time, page_table[i].mru_count);
    }
}

void relogio_init()
{
    // Initialize the clock hand
    clock_hand = NULL;

    // Insert all pages in memory into the clock
    for (int i = 0; i < REAL_MEMORY_SIZE / PAGE_SIZE; i++)
    {
        relogio_insert_page_table_entry(i);
    }
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
unsigned short relogio(unsigned short virtual_index)
{
    if (clock_hand == NULL)
    {
        printf("Relógio vazio\n");
        return -1;
    }

    page_table_entry *virtual_page = &page_table[virtual_index];

    while (1)
    {
        complexidade++;
        if (clock_hand->page_table_entry->referenced == 0)
        {
            test_aux_1++;
            unsigned short frame = clock_hand->page_table_entry->frame;
            clock_hand->page_table_entry->present = 0;
            clock_hand->page_table_entry->referenced = 0;
            clock_hand->page_table_entry->modified = 0;
            clock_hand->page_table_entry->frame = UNKNOW_PAGE;
            clock_hand->page_table_entry = virtual_page;
            clock_hand->virtual_page = virtual_index;
            clock_hand = clock_hand->next;
            return frame;
        }
        else
        {
            test_aux_2++;
            // Page not found
            clock_hand->page_table_entry->referenced = 0;
            clock_hand = clock_hand->next;
        }
    }
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
    page_acess_count++;
    unsigned int page_index = virtual_address / PAGE_SIZE;
    unsigned int page_offset = virtual_address % PAGE_SIZE;

    if (page_index >= NUM_PAGES)
    {
        printf("Erro: endereço virtual %u inválido\n", virtual_address);
        return -1;
    }
    if (page_offset >= PAGE_SIZE)
    {
        printf("Erro: offset %u inválido\n", page_offset);
        return -1;
    }

    if (!page_table[page_index].present)
    {
        // Page miss
        page_miss_count++;

        // Memory is always full, so we need to replace a page

        // Call the page replacement algorithm
        unsigned int frame = 0;
        switch (algorithm)
        {
        case RELOGIO:
            frame = relogio(page_index);
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
    page_table[page_index].referenced = 1;

    return physical_address;
}

void clock_tick()
{
    // Implementar o clock tick
    ticks++;
}

void loop(int hits_per_tick)
{
    unsigned int virtual_address = 0;
    unsigned int _physical_address;
    int i;

    do
    {
        for (i = 0; i < hits_per_tick; i++)
        {
            scanf("%u", &virtual_address);
            if (feof(stdin))
            {
                fflush(stdout);
                return;
            }
            if (virtual_address > VIRTUAL_ADDRESS_SPACE_SIZE)
            {
                printf("Endereço virtual inválido\n");
                return;
            }
            _physical_address = virtual_to_physical(virtual_address);
            if (_physical_address == -1)
            {
                printf("Erro ao converter endereço virtual para físico\n");
                return;
            }
            else if (_physical_address > REAL_MEMORY_SIZE)
            {
                printf("Erro ao converter endereço virtual para físico\n");
                return;
            }
        }
        clock_tick();
    } while (1);
}

int main(int argc, char **argv)
{
    for (int i = 0; i < NUM_PAGES; i++)
    {
        page_table[i].present = 0;
        page_table[i].frame = UNKNOW_PAGE;
    }

    // Fill the real memory with pages
    for (int i = 0; i < REAL_MEMORY_SIZE / PAGE_SIZE; i++)
    {
        page_table[i].present = 1;
        page_table[i].frame = i;
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
        else
        {
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

    int hits_per_tick = 1000;
    if (argc > 2)
    {
        hits_per_tick = atoi(argv[2]);
    }

    loop(hits_per_tick);
    printf("-------------------\n");
    printf("Page acess count: %lu\n", page_acess_count);
    printf("Page miss count: %lu\n", page_miss_count);
    printf("Ticks: %lu\n", ticks);
    printf("Test aux 1: %lu\n", test_aux_1);
    printf("Test aux 2: %lu\n", test_aux_2);
    printf("aux 1 + aux 2: %lu\n", test_aux_1 + test_aux_2);
    printf("Complexity: %lu\n", complexidade);
    print_page_table();

    switch (algorithm)
    {
    case RELOGIO:
        relogio_free();
        break;
    case AGING:
        break;
    case WSCLOCK:
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