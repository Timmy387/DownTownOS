#include "typeWriter.h"
#include "../files/fileBackend.h"
#include "../strings/string.h"
#include "../utils/screen.h"
#include "../utils/util.h"
#include "openFile.h"



int typeWriter(char * filename){
    ofile * file = openFile(filename, 1, 1);
    rowData ** savedScreen = saveScreen();
    configFile * config = getConfig();
    screenData * sd = getScreenData();
    int currow = sd->row;
    int namelen = strLen(filename);
    int i, j;
    
    if (!fileExist(filename)) {
        loadScreen(savedScreen);
        return 1;
    }
    resetScreen();
    file->toprows[0]->color = config->twTitleColor;
    for (i = 0; i < COLS / 2 - namelen / 2; i++){
        file->toprows[0]->vals[i] = ' ';
    }
    j = 0;
    for (; i < COLS / 2 + namelen / 2; i++){
        file->toprows[0]->vals[i] = filename[j++];
    }

    printFile(file);
    sd->row = file->numtoprows;
    sd->col = 0;
    moveCursor(sd->row, sd->col);
    typingLoop(filename);
    freeFile(file);
    sd->row = currow;
    loadScreen(savedScreen);
    return 0;
}


void typingLoop(char * filename){
    screenData * sd = getScreenData();
    rowData ** screen = getScreen();
    rowData * row = screen[sd->row];
    char * modified = "modified";
    char * savedcontents;
    short keyPress = 0;
    int i, j, count;
    char ch = 0;
    int saved = 1;
    char sc = 0;
    short lastcol = 0;
    while (1){
        while (keyPress == 0) keyPress = inb();
        ch = keyPress & 0xFF;
        sc = keyPress >> 8;
        if (ch == 13){
            saved = 0;
            printLiteral("\n");
        }
        else if (ch == 0x13){ // Ctrl+S
            if (!saved) {
                count = 0;
                for (i = 1; i < ROWS; i++){
                    count += screen[i]->newlineloc + 1;
                }
                savedcontents = (char*)malloc(count + 1);
                count = 0;
                for (i = 1; i < ROWS; i++){
                    j = 0;
                    while (screen[i]->vals[j]) {
                        savedcontents[count] = screen[i]->vals[j];
                        j++;
                        count++;
                    }
                    savedcontents[count++] = '\n';
                }
                savedcontents[count - 1] = 0;
                saveFile(filename, savedcontents);
                saved = 1;
            }
        }
        else if (!ch && sc == 0x48){ // up arrow
            if (sd->row > 1) {
                sd->row--;
                row = screen[sd->row];
                if (lastcol < row->newlineloc)sd->col = lastcol;
                else sd->col = row->newlineloc;
            }
            else sd->col = 0;
            moveCursor(sd->row, sd->col);
        }
        else if (!ch && sc == 0x4B){ // left arrow
            if (sd->col) sd->col--;
            else if (sd->row > 1){
                sd->row--;
                row = screen[sd->row];
                sd->col = row->newlineloc;
            }
            moveCursor(sd->row, sd->col);
            lastcol = sd->col;

        }
        else if (!ch && sc == 0x50){ // down arrow
            if (sd->row < ROWS - 1){
                sd->row++;
                row = screen[sd->row];
                if (lastcol < row->newlineloc)sd->col = lastcol;
                else sd->col = row->newlineloc;
            }
            else {
                sd->col = row->newlineloc;
            }
            moveCursor(sd->row, sd->col);
        }
        else if (!ch && sc == 0x4D){ // right arrow
            if (sd->col < row->newlineloc) sd->col++;
            else if (sd->row < ROWS - 1){
                sd->row++;
                sd->col = 0;
            }
            moveCursor(sd->row, sd->col);
            lastcol = sd->col;
        }
        else if(ch == 0x1B || ch == 0x18) return; // exit on ESC or Ctrl+X
        else if (ch){
            saved = 0;
            printChar(ch);
        }
        i = 0;
        while (modified[i]){
            if (saved) screen[0]->vals[COLS - strLen(modified) - 1 + i] = 0;
            else screen[0]->vals[COLS - strLen(modified) - 1 + i] = modified[i];
            i++;
        }
        printScreen();
        keyPress = 0;
    }
}


void printFile(ofile * file){
    int i, j;
    rowData ** screen = getScreen();
    int numrowstoprint = ROWS - file->numtoprows - file->numbotrows;
    if (file->startrow + file->numrows < numrowstoprint) numrowstoprint = file->startrow + file->numrows;
    for (i = 0; i < file->numtoprows; i++){
        freeRowData(screen[i]);
        screen[i] = file->toprows[i];
    }
    for (i = 0; i < numrowstoprint; i++){
        freeRowData(screen[i + file->numtoprows]);
        screen[i + file->numtoprows] = file->rows[i + file->startrow];
    }
    j = 0;
    for (i = ROWS - file->numbotrows; i < ROWS; i++){
        freeRowData(screen[i]);
        screen[i] = file->botrows[j++];
    }
    printScreen();
}