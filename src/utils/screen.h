#ifndef SCREEN_H
#define SCREEN_H

#define ROWS 25
#define COLS 80

typedef struct {
    short row;
    short col;
}screenData;

typedef struct {
    char * vals;
    short newlineloc;
    short color;
}rowData;

void resetScreen();

void scroll(int rows);

rowData ** saveScreen();

void freeRowData(rowData * row);

rowData * freshRow();

void loadScreen(rowData ** screenBuff);

rowData ** getScreen();

void printScreen();

screenData * getScreenData();

#endif