#include "openFile.h"
#include "../utils/util.h"
#include "../files/fileBackend.h"


ofile * openFile(char * filename, int toprows, int botrows){ // num extra top and bottom rows to leave blank for metadata later
    ofile * file = (ofile*)malloc(sizeof(ofile));
    char * contents = readFileContents(filename);
    rowData * row;
    char * tempcontents = contents;
    int i = 0;
    int j = 0;
    file->filename = (char*)malloc(strLen(filename) + 1);
    strcpy(file->filename, filename);
    file->numrows = countChar(contents, '\n') + 1;
    file->numtoprows = toprows;
    file->numbotrows = botrows;
    file->rows = (rowData**)malloc(sizeof(rowData*) * file->numrows);
    for (i = 0; i < file->numrows; i++){
        row = freshRow();
        file->rows[i] = row;
        j = 0;
        while (contents[j] && contents[j] != '\n'){
            row->vals[j] = contents[j];
            j++;
        }
        contents += j;
        if (*contents == '\n') contents++;
        row->newlineloc = j;
    }
    file->toprows = (rowData**)malloc(sizeof(rowData*) * file->numtoprows);
    file->botrows = (rowData**)malloc(sizeof(rowData*) * file->numbotrows);
    for (i = 0; i < toprows; i++) file->toprows[i] = freshRow();
    for (i = 0; i < botrows; i++) file->botrows[i] = freshRow();
    file->startrow = 0;
    free(tempcontents);
    return file;
}


void freeFile(ofile * file){
    int i;
    for (i = 0; i < file->numtoprows; i++){
        freeRowData(file->toprows[i]);
    }
    for (i = 0; i < file->numrows; i++){
        freeRowData(file->rows[i]);
    }
    for (i = 0; i < file->numbotrows; i++){
        freeRowData(file->botrows[i]);
    }
    free(file->toprows);
    free(file->rows);
    free(file->botrows);
    free(file->filename);
    free(file);
}


int saveOfile(ofile * file){
    char * contents;
    short i, j, count;
    count = 0;
    for (i = 0; i < file->numrows; i++){
        count += file->rows[i]->newlineloc + 1;
    }
    contents = (char*)malloc(count + 1);
    count = 0;
    for (i = 0; i < file->numrows; i++){
        j = 0;
        while (file->rows[i]->vals[j]){
            contents[count++] = file->rows[i]->vals[j];
            j++;
        }
        contents[count++] = '\n';
    }
    contents[count] = 0;
    saveFile(file->filename, contents);
    free(contents);
    return 0;
}