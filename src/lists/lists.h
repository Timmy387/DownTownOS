#ifndef LISTS_H
#define LISTS_H


typedef struct arrayList{
    int size;
    int capacity;
    void * elements;
    int dataSize;
} arrayList;

arrayList * newArrayList(int * count, int dataSize);

void * get(arrayList * arr, int index);

void add(arrayList * arr, void * element);

void remove(arrayList * arr, int index);

#endif