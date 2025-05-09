#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef MALLOC_H
#define MALLOC_H

typedef struct memBlock{
    unsigned short size;
    unsigned int free; // 1 if freed, 0 if allocated
    struct memBlock * next;
} memBlock;

void initHeap();

void * malloc(unsigned short size);

void free(void * ptr);

#endif