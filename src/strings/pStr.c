#include "pStr.h"
#include "../utils/util.h"
#include "../assembly/asm.h"
#include "../memory/malloc.h"
#include "../commands/commands.h"
#include "../utils/screen.h"
#include "../utils/time.h"

#define MAX_ROWS 25
#define MAX_COLS 80


pStr * initStr(char * s, int row, int col, int color){
    pStr * new;
    int len = strLen(s);
    int cap = 4;
    while (len + 1 > cap) cap *= 2;
    new = (pStr*)malloc(sizeof(pStr));
    new->str = (char*)malloc(cap);
    strcpy(new->str, s);
    new->row = row;
    new->col = col;
    new->color = color;
    new->length = len;
    new->capacity = cap;
    return new;
}

void freeStr(pStr * s){
    free(s->str);
    free(s);
}


void putStr(pStr * s){
    int i = 0;
    for (; s->str[i] != 0; i++){
        putChar(s->str[i], s->row, s->col + i, s->color);
        if (s->col + i > MAX_COLS - 1){
            s->row++;
            s->col = -i;
            if (s->row > MAX_ROWS - 1) s->row = 0;
        }
    }
}


void typeStr(pStr * s, char c){
    int slen = s->length;
    char * new;
    if (!c) moveCursor(s->row, s->col);
    else if (c == 8){ // backspace
        if (s->length > 0){
            putChar(' ', s->row, s->col + slen - 1, s->color);
            s->str[slen - 1] = 0;
            s->length--;
            moveCursor(s->row, s->col + slen - 1);
        }
    }
    else {
        if (slen + 2 >= s->capacity){
            s->capacity *= 2;
            new = (char*)malloc(s->capacity);
            strcpy(new, s->str);
            new[slen] = c;
            new[slen + 1] = 0;
            free(s->str);
            s->str = new;
        }
        else {
            s->str[slen] = c;
            s->str[slen + 1] = 0;
        }
        s->length++;
        moveCursor(s->row, s->col + slen + 1);
    }
    putStr(s);
}


void moveStr(pStr * s, int newRow, int newCol){
    int tempColor = s->color;
    s->color = s->color / 0x10;
    s->color = s->color * 10 + s->color; // make foreground same as background
    putStr(s); // color over old string
    s->color = tempColor;
    s->row = newRow;
    s->col = newCol;
    putStr(s); // reprint string in new location
}

void bounceAround(char * st, int speed){
    pStr * s = initStr(st, 0, 0, 0x01);
    int newRow = 0;
    int newCol = 0;
    int counter = 0;
    int rowDown = 1;
    int colRight = 1;
    char key = 0;
    int i;
    int j;
    char * screenBuff = saveScreen();
    // fullTime * startTime = getTime();
    // fullTime * endTime = getTime();
    resetScreen();
    while (1){
        // delay(1, 0, 0, 0);
        if (speed == 1){
            for (i = 0; i < 1500; i++) for (j = 0; j < 1500; j++);
        }
        else if (speed == 2){
            for (i = 0; i < 1200; i++) for (j = 0; j < 1200; j++);
        }
        else if (speed == 3){
            for (i = 0; i < 900; i++) for (j = 0; j < 900; j++);
        }
        else {
            loadScreen(screenBuff);
            return;
        }
        
        key = inb();
        if (key == 0x1B) {
            // free(startTime);
            // free(endTime);
            loadScreen(screenBuff);
            return;
        }
        moveStr(s, newRow, newCol);
        if (colRight) newCol++;
        else newCol--;

        counter++;
        if (counter == 5){
            counter = 0;
            if (rowDown) newRow++;
            else newRow--;
        }
        if (newRow == 0) {
            s->color = 0x09;
            rowDown = 1;
        }
        if (newCol == 0) {
            s->color = 0x02;
            colRight = 1;
        }
        if (newRow + 1 == MAX_ROWS) {
            s->color = 0x03;
            rowDown = 0;
        }
        if (newCol + s->length == MAX_COLS) {
            s->color = 0x04;
            colRight = 0;
        }

        if (newRow > MAX_ROWS - 1) newRow = 0;
        if (newCol > MAX_COLS - 1) newCol = 0;
    }
}


void hopAround(char * st, int speed){
    int timer1 = 0;
    int timer2 = 0;
    char key = 0;
    int counter = 0;
    int bounceSpeed = 32;
    short up = 1;
    int i;
    int j;
    pStr * s = initStr(st, 10, 0, 0x0F);
    char * screenBuff = saveScreen();
    resetScreen();
    putStr(s);
    while(1){
        if (speed == 1){
            for (i = 0; i < 150; i++) for (j = 0; j < 150; j++);
        }
        else if (speed == 2){
            for (i = 0; i < 100; i++) for (j = 0; j < 100; j++);
        }
        else if (speed == 3){
            for (i = 0; i < 70; i++) for (j = 0; j < 70; j++);
        }
        else {
            loadScreen(screenBuff);
            return;
        }
        key = inb();
        if (key == 0x1B) {
            loadScreen(screenBuff);
            return;
        }
        counter++;
        if (counter == bounceSpeed){
            if (up) {
                moveStr(s, s->row - 1, s->col + 1);
                bounceSpeed = bounceSpeed * 2;
            }
            else {
                moveStr(s, s->row + 1, s->col + 1);
                bounceSpeed = bounceSpeed / 2;
            }
            counter = 0;
        }
        if (bounceSpeed >= 1024){
            bounceSpeed = 512;
            up = !up;
            counter = 0;
        }
        else if (bounceSpeed <= 16){
            if (speed == 1){
                for (i = 0; i < 20000; i++) for (j = 0; j < 2000; j++);
            }
            else if (speed == 2){
                for (i = 0; i < 15000; i++) for (j = 0; j < 2000; j++);
            }
            else if (speed == 3){
                for (i = 0; i < 10000; i++) for (j = 0; j < 2000; j++);
            }
            up = !up;
            bounceSpeed = 32;
            counter = 0;
        }
    }
}


void newStr(pStr * s, char * str){
    int cap = 4;
    int newLen = strLen(str);
    char * newStr = (char*)malloc(newLen + 1);
    while (newLen + 1 > cap) cap *= 2;
    strcpy(newStr, str);
    free(s->str);
    s->str = newStr;
    s->length = strLen(newStr);
    s->capacity = cap;
}