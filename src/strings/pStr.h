#ifndef PSTR_H
#define PSTR_H


typedef struct printableString{
    char * str;
    int row;
    int col;
    int color;
    int length;
    int capacity;
} pStr;

void putStr(pStr * s);

pStr * initStr(char * s, int row, int col, int color);

void moveStr(pStr * st, int newRow, int newCol);

void typeStr(pStr * s, char c);

void bounceAround(char * s, int speed);

void hopAround(char * s, int speed);

void newStr(pStr * s, char * str);

#endif