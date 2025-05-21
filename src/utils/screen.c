#include "screen.h"
#include "util.h"
#include "../memory/malloc.h"
#include "../files/fileBackend.h"


void resetScreen(){
    int i;
    rowData ** screen = getScreen();
    screenData * sd = getScreenData();
    for (i = 0; i < ROWS; i++){
        freeRowData(screen[i]);
        screen[i] = freshRow();
    }
    sd->row = 0;
    sd->col = 0;
    moveCursor(0, 0);
}


void scroll(int rows){
    int i, j;
    rowData ** screen = getScreen();
    screenData * sd = getScreenData();
    for (i = 0; i < rows; i++){
        if (sd->row == 0) break;
        freeRowData(screen[0]);
        for (j = 1; j < ROWS; j++){
            screen[j - 1] = screen[j];
        }
        screen[ROWS - 1] = freshRow();
        sd->row--;
    }
    moveCursor(sd->row, sd->col);
    printScreen();
}


rowData ** saveScreen(){
    rowData ** screen = getScreen();
    rowData ** newScreen = (rowData**)malloc(sizeof(rowData*) * ROWS);
    rowData * row;
    rowData * newrow;
    int i, j;
    for (i = 0; i < ROWS; i++){
        newrow = (rowData*)malloc(sizeof(rowData));
        row = screen[i];
        newScreen[i] = newrow;
        newrow->color = row->color;
        newrow->newlineloc = row->newlineloc;
        newrow->vals = (char*)malloc(COLS + 1);
        for (j = 0; j < COLS; j++) newrow->vals[j] = row->vals[j];
    }
    return newScreen;
}


void loadScreen(rowData ** screen){
    rowData ** oldscreen = getScreen();
    rowData * row;
    rowData * oldrow;
    int i, j;
    for (i = 0; i < ROWS; i++){
        row = screen[i];
        oldrow = oldscreen[i];
        oldrow->color = row->color;
        oldrow->newlineloc = row->newlineloc;
        for (j = 0; j < COLS; j++) oldrow->vals[j] = row->vals[j];
        free(row->vals);
        free(row);
    }
    free(screen);
    printScreen();
}


rowData ** getScreen(){
    static rowData ** screenBuff;
    static short * fullScreen;
    static short initialized = 0;
    configFile * config = getConfig();
    int i, j;
    if (!initialized){
        initialized = 1;
        screenBuff = (rowData**)malloc(sizeof(rowData*) * ROWS);
        for (i = 0; i < ROWS; i++){
            screenBuff[i] = freshRow();
            for (j = 0; j < COLS + 1; j++){
                screenBuff[i]->vals[j] = 0;
            }
        }
    }
    return screenBuff;
}


void printScreen(){
    rowData ** screen = getScreen();
    screenData * sd = getScreenData();
    configFile * config = getConfig();
    rowData * row;
    int i;
    int j;
    char ch;
    for (i = 0; i < ROWS; i++){
        row = screen[i];
        for (j = 0; j < COLS; j++){ // these should always stop at the same point I think?
            putChar(row->vals[j], i, j, row->color);
        }
    }
}


screenData * getScreenData(){
    static screenData * data;
    static short initialized;
    short cursorPos;
    if (!initialized){
        initialized++;
        cursorPos = getCursorPos();
        data = (screenData*)malloc(sizeof(screenData));
        data->row = cursorPos >> 8;
        data->col = cursorPos & 0xFF;
    }
    return data;
}


rowData * freshRow(){
    rowData * row = (rowData*)malloc(sizeof(rowData));
    configFile * config = getConfig();
    int i;
    row->vals = (char*)malloc(COLS + 1);
    row->newlineloc = 0;
    row->color = config->color;
    for (i = 0; i < COLS + 1; i++){
        row->vals[i] = 0;
    }
    return row;
}


void freeRowData(rowData * row){
    free(row->vals);
    free(row);
}
