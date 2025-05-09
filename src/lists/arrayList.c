// #include "lists.h"
// #include "../memory/malloc.h"

// arrayList * newArrayList(int count, int dataSize){
//     int capacity = !count ? count : 10;
//     arrayList * list = (arrayList *)malloc(sizeof(arrayList));
//     if (!list) return NULL;
//     list->elements = malloc(capacity * dataSize);
//     if (!list->elements) {
//         free(list);
//         return NULL;
//     }
//     list->size = !count ? count : 0;
//     list->capacity = capacity;
//     list->dataSize = dataSize;
//     return list;
// }


// void * get(arrayList * arr, int index){
//     if (!arr || index < 0 || index >= arr->size) return NULL;
//     return (char *)arr->elements + index * arr->dataSize;
// }


// void add(arrayList * arr, void * element){
//     char * dest;
//     char * src;
//     int i = 0;
//     if (!arr || sizeof(element) != arr->dataSize) return;
//     if (arr->capacity <= arr->size){
//         // TODO: double the size of the array in this case.
//         return;
//     }
//     dest = (char *)arr->elements + arr->size * arr->dataSize;
//     src = (char *)element;
//     for (; i < arr->dataSize; i++) {
//         dest[i] = src[i];
//     }
//     arr->size++;
// }


// void remove(arrayList * arr, int index){
    
// }