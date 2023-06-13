#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define true 1
#define false 0

// Simulador de MMU (Memory Management Unit)
/**
 * Teste das implementações de Relógio, Envelhecimento e WSClock
 */

#define REAL_MEMORY_SIZE 2147483648 // Memory size of 2GB
#define VIRTUAL_MEMORY_SIZE (REAL_MEMORY_SIZE * 2) // Virtual memory size of 4GB
#define PAGE_SIZE 4096 // Page size of 4KB
#define VIRTUAL_ADDRESS_SPACE_SIZE (VIRTUAL_MEMORY_SIZE / PAGE_SIZE) // Address space size of 1M pages

#define UNKNOW_PAGE (unsigned int)-1

#define TICKS_PER_SECOND 1000000

typedef struct page_table_entry
{
    unsigned char referenced : 1;
    unsigned char modified : 1;
    unsigned char present : 1;
    unsigned char pad : 5;
    unsigned int frame;
    unsigned char mru_count;
    unsigned int last_access_time;
} page_table_entry;

page_table_entry page_table[VIRTUAL_ADDRESS_SPACE_SIZE];

unsigned long ticks = 0;
unsigned long page_miss_count = 0;
unsigned long page_acess_count = 0;
unsigned long complexidade = 0;

typedef enum
{
    RELOGIO,
    AGING,
    WSCLOCK
} page_replacement_algorithm;

page_replacement_algorithm algorithm = RELOGIO;

/**Relogio**/

typedef struct relogio_node
{
    unsigned int virtual_page;
    page_table_entry *page_table_entry;
    struct relogio_node *next;
} relogio_node;

relogio_node *clock_hand = NULL;

void relogio_insert_page_table_entry(unsigned int virtual_page)
{

    page_table_entry *page_table_entry = &page_table[virtual_page];

    relogio_node *new_node = malloc(sizeof(relogio_node));
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
    relogio_node *current = clock_hand->next;
    while (current != clock_hand)
    {
        relogio_node *next = current->next;
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

    relogio_node *node = clock_hand;
    do
    {
        printf("%d -> ", node->virtual_page);
        node = node->next;
    } while (node != clock_hand);
    printf("\n");
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

unsigned short relogio(unsigned short virtual_index)
{
    if (clock_hand == NULL)
    {
        printf("Relógio vazio\n");
        return -1;
    }

    page_table_entry *virtual_page = &page_table[virtual_index];

    do
    {
        complexidade++;
        if (clock_hand->page_table_entry->referenced == 0)
        {
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
            // Page not found
            clock_hand->page_table_entry->referenced = 0;
            clock_hand = clock_hand->next;
        }
    } while (true);
}

/**Aging**/

void aging_init()
{
    // TODO: Implementar
}

unsigned int aging()
{
    printf("Envelhecimento\n");
    // Implementar o algoritmo de envelhecimento

    return -1;
}

/**WSClock**/

typedef struct wsclock_node
{
    unsigned int virtual_page;
    page_table_entry *page_table_entry;
    struct wsclock_node *next;
} wsclock_node;

wsclock_node *wsclock_hand = NULL;
unsigned int tau = 0;

typedef struct tick_data
{
    unsigned long tick;
    unsigned long page_miss_count;
    unsigned long page_acess_count;
    unsigned long complexidade;
} tick_data;

tick_data current_tick_data;
tick_data last_tick_data;

void wsclock_insert_page_table_entry(unsigned int virtual_page)
{
    page_table_entry *page_table_entry = &page_table[virtual_page];

    wsclock_node *new_node = malloc(sizeof(wsclock_node));
    new_node->page_table_entry = page_table_entry;
    new_node->virtual_page = virtual_page;

    if (wsclock_hand == NULL)
    {
        wsclock_hand = new_node;
        wsclock_hand->next = wsclock_hand;
    }
    else
    {
        new_node->next = wsclock_hand->next;
        wsclock_hand->next = new_node;
    }
}

void wsclock_init()
{
    wsclock_hand = NULL;
    tau = 0;
    current_tick_data.tick = 0;
    current_tick_data.page_miss_count = 0;
    current_tick_data.page_acess_count = 0;
    current_tick_data.complexidade = 0;

    last_tick_data.tick = 0;
    last_tick_data.page_miss_count = 0;
    last_tick_data.page_acess_count = 0;
    last_tick_data.complexidade = 0;

    // Insert all pages in memory into the clock
    for (int i = 0; i < REAL_MEMORY_SIZE / PAGE_SIZE; i++)
    {
        wsclock_insert_page_table_entry(i);
    }
}

unsigned int wsclock(unsigned int virtual_index)
{
    
}

void print_page_table()
{
    printf("Página\tPresente\tQuadro\tReferenciada\tModificada\tTempo de último acesso\tContador MRU\n");
    for (int i = 0; i < VIRTUAL_ADDRESS_SPACE_SIZE; i++)
    {
        printf("%u\t\t%u\t\t%u\t\t%u\t\t\t%u\t\t%u\t\t\t\t%u\n", i, page_table[i].present, page_table[i].frame, page_table[i].referenced, page_table[i].modified, page_table[i].last_access_time, page_table[i].mru_count);
    }
}

unsigned int virtual_to_physical(unsigned int virtual_address, unsigned int time)
{
    page_acess_count++;
    unsigned int page_index = virtual_address / PAGE_SIZE;
    unsigned int page_offset = virtual_address % PAGE_SIZE;

    if (page_index >= VIRTUAL_ADDRESS_SPACE_SIZE)
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
            frame = wsclock(page_index);
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

void set_bit_r_to_zero()
{
    for (int i = 0; i < VIRTUAL_ADDRESS_SPACE_SIZE; i++)
    {
        complexidade++;
        page_table[i].referenced = 0;
    }
}

void recalculate_tau()
{
        
    
}

void clock_tick()
{
    // Implementar o clock tick
    ticks++;

    switch (algorithm)
    {
    case RELOGIO:
        set_bit_r_to_zero();
        break;
    case AGING:
        break;
    case WSCLOCK:
        recalculate_tau();
    }
}

#define M_PI 3.14159265358979323846

double generateNormalRandomNumber(double mean, double stdDev)
{
    double u1 = 0.0, u2 = 0.0;
    u1 = rand() * (1.0 / RAND_MAX);
    u2 = rand() * (1.0 / RAND_MAX);
    double z0 = sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
    return z0 * stdDev + mean;
}

void loop(int hits_per_tick)
{
    unsigned int virtual_address = 0;
    unsigned int _physical_address;
    int j;
    int i;
    srand(0);

    double mean = VIRTUAL_MEMORY_SIZE / 2;
    double stdDev = VIRTUAL_MEMORY_SIZE / 10;

    do
    {
        for (j = 0; j < hits_per_tick; j++)
        {
            virtual_address = generateNormalRandomNumber(mean, stdDev);

            if (i > 100) {
                return;
            }

            if (virtual_address >= VIRTUAL_MEMORY_SIZE)
            {
                printf("Erro: Endereço virtual %u inválido\n", virtual_address);
                return;
            }
            _physical_address = virtual_to_physical(virtual_address, ticks);
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
        i += 1;
    } while (1);
}

int main(int argc, char **argv)
{
    for (int i = 0; i < VIRTUAL_ADDRESS_SPACE_SIZE; i++)
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
    printf("Acessos por tick: %d\n", hits_per_tick);
    printf("Complexity: %lu\n", complexidade);

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
     * ~Calcular a complexidade dos algoritmos toda vez que ele tenha que tomar alguma decisão
     * ~Calcular a quantidade de Page Miss
     *
     * Testar com diferentes tempos de acesso*
     * ~Testar com diferentes desvios nos valores aleatórios
     * Implementar uma forma de definir a seed
     * Testar com diferentes Algoritmos de Substituição de Páginas
     *
     * ~Para a entrada, usar um arquivo de texto com os endereços virtuais a serem acessados,
     * ~feitos em outra linguagem que possua uma função aleatória de distribuição normal
     *
     * Para as interrupções e para os tempos usar o i da iteração do loop
     * Para cada loop realizar X acessos a memória*
     * e a cada X acessos, realizar uma interrupção
     *
     * Para encontrar o tau, checar o ultimo tick, para variar o tau dinamicamente
     * 
     * Usar uma memória real de 2GB e uma memória virtual de 4GB
     * Usar um tick real de algum processador real
     * 
     * Refazer os números aleatórios em C
     * 
     */

    return 0;
}