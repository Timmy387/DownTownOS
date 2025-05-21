#ifndef UTIL_H
#define UTIL_H

int mod(int a, int b);

int strLen(char * s);

int strEqual(char * s1, char * s2);

void strcpy(char * dest, char * src);

void putChar(char c, int row, int col, int color);

int atoi(char * s);

int htod(char * s);

void toLower(char * s);

int countChar(char * contents, char c);

#endif