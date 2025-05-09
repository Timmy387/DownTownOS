#ifndef FILES_H
#define FILES_H

void moveToHome();

short rm(char * filename);

int mkdir(char * name);

void ls();

int cd(char * dirname);

void pwd();

int touch(char * name);

#endif