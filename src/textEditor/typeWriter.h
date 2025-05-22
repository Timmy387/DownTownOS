#ifndef TYPEWRITER_H
#define TYPEWRITER_H

#define SCREENSIZE ROWS * COLS

int typeWriter(char * filename);

void typingLoop(ofile * file);

void printFile(ofile * file);

void copyRow(rowData * dest, rowData * src);

void typeChar(ofile * file, char c);

void backspaceHandler(ofile * file);

void enterHandler(ofile * file);

#endif