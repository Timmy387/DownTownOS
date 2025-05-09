#ifndef STRING_H
#define STRING_H

typedef struct String{
    int len;
    char * s;
} String;

String * newString(char * s);

void freeString(String * s);

String * currentDir();

void addChar(String * s, char c);

int print(String * str);

int printChar(char c);

int printLiteral(char * str);

String ** split(String * str, char c);

void concat(String * dest, char * src);

int stringEqual(String * s1, String * s2);

#endif