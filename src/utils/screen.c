#include "screen.h"
#include "util.h"
#include "../memory/malloc.h"
#include "../files/fileBackend.h"


void resetScreen(){
    int row;
    int col;
    unsigned short w;
    char c;
    char color;
    unsigned short * screenBuff = (unsigned short*)malloc(ROWS * COLS * 2);
    readInScreen(screenBuff);
    for (row = 0; row < 25; row++){
        for (col = 0; col < 80; col++) {
            w = screenBuff[row * 80 + col];
            c = w & 0xFF;
            color = (w >> 8) & 0xFF;
            if (c != ' '){
                putChar(0, row, col, 0x0F);
            }
        }
    }
    hideCursor();
    free(screenBuff);
}


void scroll(int rows){
    int row;
    int col;
    char * screenBuff = getScreen();
    screenData * sd = getScreenData();
    while (sd->row > 0 && rows > 0){
        for (row = 1; row < ROWS; row++){
            for (col = 0; col < COLS; col++){
                screenBuff[(row - 1) * COLS + col] = screenBuff[row * COLS + col];
            }
        }
        for (col = 0; col < COLS; col++) screenBuff[(ROWS - 1) * COLS + col] = 0;
        sd->row--;
        rows--;
    }
    moveCursor(sd->row, sd->col);
    printScreen();
}


char * saveScreen(){
    char * screenBuff = (char*)malloc(ROWS * COLS + 1);
    char * screen = getScreen();
    int i;
    for (i = 0; i < ROWS * COLS; i++){
        screenBuff[i] = screen[i];
    }
    return screenBuff;
}


void loadScreen(char * screenBuff){
    int i;
    char * screen = getScreen();
    for (i = 0; i < ROWS * COLS; i++){
        screen[i] = screenBuff[i];
    }
    printScreen();
    free(screenBuff);
}


rowData ** getScreen1(){
    static rowData ** screenBuff;
    static short * fullScreen;
    static short initialized = 0;
    int i, j;
    if (!initialized){
        initialized = 1;
        screenBuff = (rowData**)malloc(sizeof(rowData*) * ROWS);
        for (i = 0; i < ROWS; i++){
            screenBuff[i] = (rowData*)malloc(sizeof(rowData));
            screenBuff[i]->vals = (char*)malloc(COLS);
            screenBuff[i]->newlineloc = 0;
            screenBuff[i]->color = 0x0F;
            for (j = 0; j < COLS; j++){
                screenBuff[i]->vals[j] = 0;
            }
        }
    }
    return screenBuff;
}


char * getScreen(){
    static char * screenBuff;
    static short * fullScreen;
    static short initialized = 0;
    int i;
    if (!initialized){
        initialized = 1;
        screenBuff = (char*)malloc(ROWS * COLS + 1);
        fullScreen = (short*)malloc(ROWS * COLS * sizeof(short));
        readInScreen(fullScreen);
        for (i = 0; i < ROWS * COLS; i++) screenBuff[i] = ' ';
        screenBuff[i] = 0;
        free(fullScreen);
    }
    return screenBuff;
}


void printScreen1(){
    // rowData ** screenBuff = getScreen();
    screenData * sd = getScreenData();

}


void printScreen(){
    char * screenBuff = getScreen();
    screenData * sd = getScreenData();
    configFile * config = getConfig();
    int i;
    char ch;
    for (i = 0; i < ROWS * COLS; i++){
        ch = screenBuff[i];
        if (ch != '\n') putChar(ch, i / 80, mod(i, 80), config->color);
        else putChar(0, i / 80, mod(i, 80), config->color);
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