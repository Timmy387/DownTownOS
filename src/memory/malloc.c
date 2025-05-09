#include "malloc.h"
#include "../utils/util.h"

#ifndef HEAP_SIZE
#define HEAP_SIZE 0x8000
#define HEAP_END 0xFFFF
#define HEAP_START (unsigned)(HEAP_END - HEAP_SIZE + 1)
#define BLK_SIZE sizeof(memBlock)
#endif


void initHeap(){
    int i;
    unsigned char * start = (unsigned char*)HEAP_START;
    memBlock * head;
    memBlock * tail;
    head = (memBlock*)(HEAP_START);
    tail = (memBlock*)(HEAP_END - sizeof(memBlock));
    head->size = HEAP_SIZE - BLK_SIZE * 2;
    head->free = 1;
    head->next = tail;
    tail->size = 0;
    tail->free = 0;
    tail->next = NULL;
    for (i = BLK_SIZE; i < HEAP_SIZE - BLK_SIZE; i++){
        start[i] = 0;
    }
}


void * malloc(unsigned short size){
    char key;
    void * p = 0;
    int space = 0;
    int i;
    char * error = "Memory Error";
    char * errorHint = "[Press esc to reset machine]";
    memBlock * startFree = (memBlock*)HEAP_START;
    memBlock * endFree;
    memBlock * nextBlock;

    if (size & 1) size++;

    while (startFree && space < size + BLK_SIZE){
        while (startFree && !startFree->free) startFree = startFree->next;
        endFree = startFree;
        while (endFree->next && endFree->free){
            space += endFree->size + BLK_SIZE;
            endFree = endFree->next;
        }
        if (space > size + BLK_SIZE) break;
        startFree = endFree;
        space = 0;
    }

    if (startFree && endFree) {
        startFree->size = size;
        startFree->free = 0;
        startFree->next = endFree;
        if (startFree != endFree && space > size + BLK_SIZE * 2 + 2){
            nextBlock = (memBlock*)((unsigned char *)startFree + BLK_SIZE + size);
            nextBlock->size = space - size - BLK_SIZE * 2;
            nextBlock->free = 1;
            nextBlock->next = endFree;
            startFree->next = nextBlock;
        }
        p = (void*)((unsigned char*)startFree + BLK_SIZE);
    }
    if (!p || (unsigned short)p < HEAP_START || (unsigned short)p > HEAP_END){
        hideCursor();
        for (i = 0; i < 80 * 25; i++) putChar(' ', i / 80, mod(i, 80), 0x0F);
        i = 0;
        while (error[i]) putChar(error[i++], 11, 34 + i, 0x0F);
        i = 0;
        while (errorHint[i]) putChar(errorHint[i++], 12, 26 + i, 0x0F);
        while(key != 0x1B) key = inb();
        asm("int 0x19");
    }
    return p;
}


// void * bestFitMalloc(unsigned short size);


void free(void * ptr){
    memBlock * b = (memBlock*)((char*)ptr - BLK_SIZE);
    memBlock * n = b->next;
    if (!ptr || b->free) return;
    if ((int)ptr - sizeof(memBlock) < HEAP_START) return;
    b->free = 1;
    while (n->free){
        b->size += BLK_SIZE + n->size;
        b->next = n->next;
        n = b->next;
    }
}