#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define true 1
#define false 0

// Simulador de MMU (Memory Management Unit)
/**
 * Teste das implementações de Relógio, Envelhecimento e WSClock
 */

#ifdef DEBUG
#define REAL_MEMORY_SIZE 32768 // Memory size of 32KB is 32768
#else
#define REAL_MEMORY_SIZE 2147483648 // Memory size of 2GB is 2147483648
#endif
#define VIRTUAL_MEMORY_SIZE (REAL_MEMORY_SIZE * 2) // Virtual memory size of 4GB
#define PAGE_SIZE 4096                             // Page size of 4KB
#define VIRTUAL_ADDRESS_SPACE_SIZE \
  (VIRTUAL_MEMORY_SIZE / PAGE_SIZE) // Address space size of 1M pages

#define UNKNOW_PAGE (REAL_MEMORY_SIZE / PAGE_SIZE)

typedef struct
{
  unsigned char referenced : 1;
  unsigned char modified : 1;
  unsigned char present : 1;
  unsigned char pad : 5;
  unsigned int frame;
  unsigned char mru_count;
  unsigned int last_access_time;
  unsigned int page_number;
} page_table_entry;

page_table_entry page_table[VIRTUAL_ADDRESS_SPACE_SIZE];

void print_page_table()
{
  printf("Página\tPresente\tQuadro\t\tReferenciada\tModificada\tTempo de "
         "último acesso\tContador MRU\n");
  for (int i = 0; i < VIRTUAL_ADDRESS_SPACE_SIZE; i++)
  {
    printf("%u\t%u\t\t%u\t\t%u\t\t%u\t\t%u\t\t\t%u\n", i, page_table[i].present,
           page_table[i].frame, page_table[i].referenced,
           page_table[i].modified, page_table[i].last_access_time,
           page_table[i].mru_count);
  }
}

unsigned long interrupts = 0;
unsigned long virtual_time = 0;

typedef struct tick_data
{
  unsigned long page_miss_count;
  unsigned long page_acess_count;
  unsigned long complexidade;
  unsigned long tau;
} tick_data;

tick_data current_tick_data;
tick_data last_tick_data;

typedef enum
{
  RELOGIO,
  AGING,
  WSCLOCK
} page_replacement_algorithm;

page_replacement_algorithm algorithm = WSCLOCK;

/**Relogio**/

#pragma region Relogio

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

unsigned short relogio_replace_page(page_table_entry *virtual_page,
                                    unsigned short virtual_index)
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

unsigned short relogio(unsigned short virtual_index)
{
  if (clock_hand == NULL)
  {
    printf("Relógio vazio\n");
    exit(1);
  }

  page_table_entry *virtual_page = &page_table[virtual_index];

  do
  {
    current_tick_data.complexidade++;
    if (clock_hand->page_table_entry->referenced == 0)
    {
      return relogio_replace_page(virtual_page, virtual_index);
    }
    else
    {
      clock_hand->page_table_entry->referenced = 0;
      clock_hand = clock_hand->next;
    }
  } while (true);
}

#pragma endregion

/**Aging**/

#pragma region Aging

typedef struct aging_node
{
  unsigned int virtual_page;
  page_table_entry *page_table_entry;
} aging_node;

#define AGING_LIST_SIZE REAL_MEMORY_SIZE / PAGE_SIZE

aging_node aging_list[AGING_LIST_SIZE];
unsigned int aging_list_index = 0;

void print_aging_list()
{
  printf("Lista de envelhecimento (index: %u): \n", aging_list_index);
  for (int i = 0; i < AGING_LIST_SIZE; i++)
  {
    printf("%u[%u](%u) ", aging_list[i].virtual_page, aging_list[i].page_table_entry->frame, aging_list[i].page_table_entry->mru_count);
  }
  printf("\n");
}

int compare_aging_nodes(const void *a, const void *b)
{
  aging_node *aging_node_a = (aging_node *)a;
  aging_node *aging_node_b = (aging_node *)b;

  if (aging_node_a->page_table_entry->mru_count >
      aging_node_b->page_table_entry->mru_count)
  {
    return 1;
  }
  else if (aging_node_a->page_table_entry->mru_count <
           aging_node_b->page_table_entry->mru_count)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

void age_page_table()
{
  for (int i = 0; i < VIRTUAL_ADDRESS_SPACE_SIZE; i++)
  {
    current_tick_data.complexidade++;
    page_table[i].mru_count >>= 1;
    if (page_table[i].referenced)
    {
      page_table[i].mru_count |= 0x80;
    }
    else
    {
      page_table[i].mru_count &= 0x7F;
    }
    page_table[i].referenced = 0;
  }

  aging_list_index = 0;
  unsigned int size = 0;

  for (int i = 0; i < VIRTUAL_ADDRESS_SPACE_SIZE; i++)
  {
    current_tick_data.complexidade++;
    if (page_table[i].present == 1)
    {
      aging_list[size].virtual_page = i;
      aging_list[size].page_table_entry = &page_table[i];
      size++;
    }
  }
  if (size < AGING_LIST_SIZE || size > AGING_LIST_SIZE)
  {
    print_page_table();
    print_aging_list();
    printf("Erro ao criar lista de envelhecimento\n");
    exit(1);
  }

#ifdef DEBUG
  print_aging_list();
#endif

  qsort(aging_list, size - 1, sizeof(aging_node), compare_aging_nodes);

#ifdef DEBUG
  print_aging_list();
#endif

  return;
}

void aging_init()
{
  for (int i = 0; i < VIRTUAL_ADDRESS_SPACE_SIZE; i++)
  {
    page_table[i].mru_count = 0;
    page_table[i].last_access_time = 0;
  }
}

#define ULONG_MAX 4294967295

int select_victim_page()
{
  int victim_page = VIRTUAL_ADDRESS_SPACE_SIZE;
  while (victim_page == VIRTUAL_ADDRESS_SPACE_SIZE && aging_list_index < AGING_LIST_SIZE)
  {
    current_tick_data.complexidade++;
    if (aging_list[aging_list_index].page_table_entry->present == 1)
    {
      victim_page = aging_list[aging_list_index].virtual_page;
    }
    aging_list_index++;
  }

  if (page_table[victim_page].frame == UNKNOW_PAGE)
  {
    print_page_table();
    print_aging_list();
    printf("Erro: Não há páginas na memória\n");
    exit(1);
  }

  if (aging_list_index >= AGING_LIST_SIZE)
  {
#ifdef DEBUG
    print_page_table();
    print_aging_list();
    printf("Lista de envelhecimento cheia, resetando\n");
#endif
    age_page_table();
    return select_victim_page();
  }

  return victim_page;
}

unsigned int aging(unsigned int virtual_index)
{
  int victim_page = select_victim_page();

  if (victim_page == -1)
  {
    printf("Erro: Não há páginas na memória\n");
    exit(1);
  }

  page_table[victim_page].present = 0;
  page_table[victim_page].referenced = 0;
  page_table[victim_page].modified = 0;
  unsigned int frame = page_table[victim_page].frame;
  page_table[victim_page].frame = UNKNOW_PAGE;

  if (frame == UNKNOW_PAGE)
  {
    printf("Erro: Não há páginas na memória\n");
    exit(1);
  }

  return frame;
}

#pragma endregion

/**WSClock**/

#pragma region WSClock

typedef struct wsclock_node
{
  unsigned int virtual_page;
  page_table_entry *page_table_entry;
  struct wsclock_node *next;
} wsclock_node;

wsclock_node *wsclock_hand = NULL;
unsigned int tau = 100; // Tau is the time interval to consider a page old

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

void wsclock_free()
{
  if (wsclock_hand == NULL)
  {
    return;
  }

  // Traverse the list and free each node
  wsclock_node *current = wsclock_hand->next;
  while (current != wsclock_hand)
  {
    wsclock_node *next = current->next;
    free(current);
    current = next;
  }

  // Free the last node and reset the clock hand
  free(wsclock_hand);
  wsclock_hand = NULL;
}

void wsclock_init()
{
  wsclock_hand = NULL;

  // Insert all pages in memory into the clock
  for (int i = 0; i < REAL_MEMORY_SIZE / PAGE_SIZE; i++)
  {
    wsclock_insert_page_table_entry(i);
  }
}

unsigned short wsclock_replace_page(page_table_entry *virtual_page,
                                    unsigned short virtual_index)
{
  unsigned short frame = wsclock_hand->page_table_entry->frame;
  wsclock_hand->page_table_entry->present = 0;
  wsclock_hand->page_table_entry->referenced = 0;
  wsclock_hand->page_table_entry->modified = 0;
  wsclock_hand->page_table_entry->frame = UNKNOW_PAGE;
  wsclock_hand->page_table_entry = virtual_page;
  wsclock_hand->virtual_page = virtual_index;
  wsclock_hand = wsclock_hand->next;
  return frame;
}

unsigned int wsclock(unsigned int virtual_index)
{
  if (wsclock_hand == NULL)
  {
    printf("WSClock vazio\n");
    exit(1);
  }

  page_table_entry *virtual_page = &page_table[virtual_index];

  do
  {
    wsclock_hand = wsclock_hand->next;
    current_tick_data.complexidade++;
    if (wsclock_hand->page_table_entry->referenced == 0)
    {
      unsigned int page_age = virtual_time - virtual_page->last_access_time;
      if (page_age >= tau)
      {
        if (virtual_page->modified == 0) // página não modificada
        {
          return wsclock_replace_page(virtual_page, virtual_index);
        }
      }
      else
      {
        virtual_time++;
      }
    }
    else
    {
      wsclock_hand->page_table_entry->referenced = 0;
      wsclock_hand->page_table_entry->last_access_time = virtual_time;
    }

  } while (true);
}

#pragma endregion

unsigned int virtual_to_physical(unsigned int virtual_address)
{
  current_tick_data.page_acess_count++;
  unsigned int page_index = virtual_address / PAGE_SIZE;
  unsigned int page_offset = virtual_address % PAGE_SIZE;

  if (page_index >= VIRTUAL_ADDRESS_SPACE_SIZE)
  {
    printf("Erro: endereço virtual %u inválido\n", virtual_address);
    exit(1);
  }
  if (page_offset >= PAGE_SIZE)
  {
    printf("Erro: offset %u inválido\n", page_offset);
    exit(1);
  }

  unsigned int frame = page_table[page_index].frame;

  if (!page_table[page_index].present)
  {
    // Page miss
    current_tick_data.page_miss_count++;

    // Memory is always full, so we need to replace a page

    // Call the page replacement algorithm
    switch (algorithm)
    {
    case RELOGIO:
      frame = relogio(page_index);
      break;
    case AGING:
      frame = aging(page_index);
      break;
    case WSCLOCK:
      frame = wsclock(page_index);
      break;
    }

    // Load the page into memory
    page_table[page_index].frame = frame;

    // Update the page table
    page_table[page_index].present = 1;
    page_table[page_index].referenced = 1;
    page_table[page_index].modified = 0;
    page_table[page_index].last_access_time = virtual_time;

    // Return the physical address
    unsigned int physical_address = frame * PAGE_SIZE + page_offset;

    if (physical_address >= REAL_MEMORY_SIZE)
    {
      printf("Erro: endereço físico %u inválido\n", physical_address);
      exit(1);
    }
    return physical_address;
  }

  unsigned int physical_address =
      page_table[page_index].frame * PAGE_SIZE + page_offset;
  page_table[page_index].referenced = 1;
  page_table[page_index].last_access_time = virtual_time;
  return physical_address;
}

void set_bit_r_to_zero()
{
  for (int i = 0; i < VIRTUAL_ADDRESS_SPACE_SIZE; i++)
  {
    current_tick_data.complexidade++;
    page_table[i].referenced = 0;
  }
}

int tau_variation = 0;

void adaptive_variation()
{
  // Calcule a diferença entre o resultado do experimento atual e a métrica de
  // referência
  double current_rate = (double)current_tick_data.page_miss_count /
                        (double)current_tick_data.page_acess_count;

  if (last_tick_data.page_acess_count == 0)
  {
    return;
  }
  double last_rate = (double)last_tick_data.page_miss_count /
                     (double)last_tick_data.page_acess_count;

  double variation = current_rate - last_rate;

  short variation_sign = 0;
  if (variation > 0)
  {
    variation_sign = 1;
  }
  else if (variation < 0)
  {
    variation_sign = -1;
  }

  short tau_variation_sign = 1;
  if (tau_variation > 0)
  {
    tau_variation_sign = 1;
  }
  else if (tau_variation < 0)
  {
    tau_variation_sign = -1;
  }

  // Objetivo reduzir a taxa de page miss
  // Se a variação for positiva, a taxa de page miss está aumentando
  // Se a variação for negativa, a taxa de page miss está diminuindo

  // Se a variação for positiva, a tau_variation deve ser positiva
  // Se a variação for negativa, a tau_variation deve ser negativa

  if (variation_sign == tau_variation_sign)
  {
    // Se a variação e a tau_variation tiverem o mesmo sinal, aumente a tau com base na diferença
    tau_variation += variation * 1000;
  }
  else
  {
    // Se a variação e a tau_variation tiverem sinais opostos, diminua a tau com base na diferença
    tau_variation -= variation * 1000;
  }

  if (abs(tau_variation) < 1000)
  {
    tau_variation = 1000 * tau_variation_sign;
  }

  // Atualize o valor de tau
  tau += tau_variation;
}

void clock_interrupt()
{
  interrupts++;

  switch (algorithm)
  {
  case RELOGIO:
    set_bit_r_to_zero();
    break;
  case AGING:
    age_page_table();
    break;
  case WSCLOCK:
    set_bit_r_to_zero();
    adaptive_variation();
    break;
  }

  last_tick_data.complexidade = current_tick_data.complexidade;
  last_tick_data.page_miss_count = current_tick_data.page_miss_count;
  last_tick_data.page_acess_count = current_tick_data.page_acess_count;
}

#define M_PI 3.14159265358979323846

double generateNormalRandomNumber(double mean, double stdDev)
{
  double u1 = 0.0, u2 = 0.0;
  u1 = rand() * (1.0 / RAND_MAX);
  u2 = rand() * (1.0 / RAND_MAX);
  double z0 = sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
  double result = z0 * stdDev + mean;
  if (result < 0)
  {
    result = 0;
  }
  else if (result >= VIRTUAL_MEMORY_SIZE)
  {
    result = VIRTUAL_MEMORY_SIZE - 1;
  }
  return result;
}

void loop(int acessos_por_interrupcao, int num_interrupts, int stdDevMultiplier,
          int seed)
{
  unsigned int virtual_address = 0;
  unsigned int _physical_address;
  int i;
  int j;

  srand(seed);

  double mean = REAL_MEMORY_SIZE;
  double stdDev = stdDevMultiplier * ((double)VIRTUAL_MEMORY_SIZE / 100.0);

  for (i = 1; i <= num_interrupts; i++)
  {
    for (j = 0; j < acessos_por_interrupcao; j++)
    {
      virtual_address = generateNormalRandomNumber(mean, stdDev);
      _physical_address = virtual_to_physical(virtual_address);
      if (_physical_address > REAL_MEMORY_SIZE)
      {
        printf("Erro ao traduzir endereço virtual %u para físico %u\n",
               virtual_address, _physical_address);
      }
      virtual_time++;
    }
    clock_interrupt();
  }
}

int main(int argc, char **argv)
{
  for (int i = 0; i < VIRTUAL_ADDRESS_SPACE_SIZE; i++)
  {
    page_table[i].present = 0;
    page_table[i].frame = UNKNOW_PAGE;
    page_table[i].referenced = 0;
    page_table[i].modified = 0;
    page_table[i].last_access_time = 0;
    page_table[i].page_number = i;
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
    age_page_table();
    break;
  case WSCLOCK:
    wsclock_init();
    break;
  }

  current_tick_data.page_miss_count = 0;
  current_tick_data.page_acess_count = 0;
  current_tick_data.complexidade = 0;

  last_tick_data.page_miss_count = 0;
  last_tick_data.page_acess_count = 0;
  last_tick_data.complexidade = 0;

  int acessos_por_interrupcao = 3000;
  int num_interrupts = 100;
  int seed = 1;
  int stdDevMultiplier = 10;
  tau = 100;
  if (argc > 2)
  {
    stdDevMultiplier = atoi(argv[2]);
  }
  if (argc > 3)
  {
    acessos_por_interrupcao = atoi(argv[3]);
  }
  if (argc > 4)
  {
    seed = atoi(argv[4]);
  }
  if (argc > 5)
  {
    num_interrupts = atoi(argv[5]);
  }
  if (argc > 6)
  {
    tau = atoi(argv[6]);
  }

#ifdef DEBUG
  print_page_table();
#endif

  loop(acessos_por_interrupcao, num_interrupts, stdDevMultiplier, seed);

  printf("-------------------\n");
  printf("*Algorithm: %s\n", algorithm == RELOGIO ? "Relógio"
                             : algorithm == AGING ? "Aging"
                                                  : "WSClock");
  printf("*Page acess count: %lu\n", current_tick_data.page_acess_count);
  printf("*Interrupções: %lu\n", interrupts);
  printf("*Acessos por tick: %d\n", acessos_por_interrupcao);
  printf("*Seed: %d\n", seed);
  printf("*Standard Deviation: %d (%d per cent)\n",
         stdDevMultiplier * VIRTUAL_MEMORY_SIZE / 100, stdDevMultiplier);
  printf("*tau: %d\n", tau);
  printf("Complexity: %lu\n", current_tick_data.complexidade);
  printf("Page miss count: %lu\n", current_tick_data.page_miss_count);
  printf("Page fault rate: %f\n",
         (double)current_tick_data.page_miss_count /
             (double)current_tick_data.page_acess_count);

#ifdef DEBUG
  print_page_table();
#endif

  switch (algorithm)
  {
  case RELOGIO:
    relogio_free();
    break;
  case AGING:
    break;
  case WSCLOCK:
    wsclock_free();
    break;
  }

  /**
   * Ideia dos testes:
   * ~Calcular a complexidade dos algoritmos toda vez que ele tenha que tomar
   * alguma decisão ~Calcular a quantidade de Page Miss
   *
   * Testar com diferentes tempos de acesso*
   * ~Testar com diferentes desvios nos valores aleatórios
   * ~Implementar uma forma de definir a seed
   * ~Testar com diferentes Algoritmos de Substituição de Páginas
   *
   * ~Para a entrada, usar um arquivo de texto com os endereços virtuais a serem
   * acessados, ~feitos em outra linguagem que possua uma função aleatória de
   * distribuição normal
   *
   * ~Para as interrupções e para os tempos usar o i da iteração do loop
   * ~Para cada loop realizar X acessos a memória*
   * ~e a cada X acessos, realizar uma interrupção
   *
   * ~Para encontrar o tau, checar o ultimo tick, para variar o tau
   * dinamicamente
   *
   * ~Usar uma memória real de 2GB e uma memória virtual de 4GB
   * ~Usar um tick real de algum processador real
   *
   * ~Refazer os números aleatórios em C
   *
   */

  return 0;
}