#ifndef TYPEWRITER_H
#define TYPEWRITER_H

#define SCREENSIZE ROWS * COLS

int typeWriter(char * filename);

void typingLoop(char * filename);

void printFile(ofile * file);

#endif