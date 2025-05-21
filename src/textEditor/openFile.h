#ifndef OPENFILE_H
#define OPENFILE_H

#include "../utils/screen.h"


typedef struct openFile{
    char * filename;
    rowData ** rows;
    rowData ** toprows;
    rowData ** botrows;
    short numrows;
    short numtoprows;
    short numbotrows;
    short startrow;
}ofile;

ofile * openFile(char * filename, int toprows, int botrows);

void freeFile(ofile * file);

#endif