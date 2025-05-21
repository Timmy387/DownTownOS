#include "util.h"
#include "../assembly/asm.h"


int mod(int a, int b) {
    return a - (a / b) * b;
}

int strLen(char * s){
    int i = 0;
    if (!s) return 0;
    while (s[i]) i++;
    return i;
}


int strEqual(char * s1, char * s2){
    int s1len;
    int s2len;
    int i = 0;
    if (!s1 || !s2) return 0;
    s1len = strLen(s1);
    s2len = strLen(s2);
    if (s1len != s2len) return 0;
    for (; i < s1len; i++){
        if (s1[i] != s2[i]) return 0;
    }
    return 1;
}


void strcpy(char * dest, char * src){
    int i = 0;
    while (src[i]){
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
    // while (*src) *(dest++) = *(src++);
    // *dest = 0;
}



void putChar(char c, int row, int col, int color){
    int offset = (row * 80 + col) * 2;
    putInMemory(0xB800, offset, c);
    putInMemory(0xB800, offset + 1, color);
}


int atoi(char * s){
    int i = 0;
    int num = 0;
    while (s[i] && s[i] >= '0' && s[i] <= '9'){
        num *= 10;
        num += s[i] - '0';
        i++;
    }
    return num;
}


int htod(char * s){
    int i = 0;
    int num = 0;
    toLower(s);
    while (s[i] && ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'f'))){
        if (s[i] >= '0' && s[i] <= '9'){
            num *= 16;
            num += s[i] - '0';
        }
        else{
            num *= 16;
            num += s[i] - 'a' + 10;
        }
        i++;
    }
    return num;
}


void toLower(char *s) {
    while (*s) {
        if (*s >= 'A' && *s <= 'Z') {
            *s = *s | 0x20;
        }
        s++;
    }
}


int countChar(char * contents, char c){
    int count = 0;
    while (*contents){
        if (*contents == c) count++;
        contents++;
    }
    return count;
}