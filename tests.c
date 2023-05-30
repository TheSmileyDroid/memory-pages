#include <stdlib.h>
#include <stdio.h>

struct page
{
    unsigned char referenced : 1;
    unsigned char modified : 1;
    unsigned char present : 1;
    unsigned char pad : 5;
    unsigned short frame;
    unsigned char mru_count;
    unsigned short last_access_time;
};

int main(int argc, char const *argv[])
{
    printf("unsigned char size: %llu\n", sizeof(unsigned char));
    printf("unsigned short size: %llu\n", sizeof(unsigned short));
    printf("unsigned int size: %llu\n", sizeof(unsigned int));
    printf("unsigned long size: %llu\n", sizeof(unsigned long));
    printf("page size: %llu\n", sizeof(struct page));

    return 0;
}
