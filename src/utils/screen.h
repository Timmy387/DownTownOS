#ifndef SCREEN_H
#define SCREEN_H

#define ROWS 25
#define COLS 80

typedef struct {
    short row;
    short col;
}screenData;

void resetScreen();

void scroll(int rows);

char * saveScreen();

void loadScreen(char * screenBuff);

char * getScreen();

void printScreen();

screenData * getScreenData();

#endif