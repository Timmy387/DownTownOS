#include "typeWriter.h"
#include "../files/fileBackend.h"
#include "../strings/string.h"
#include "../utils/screen.h"
#include "../utils/util.h"
#include "openFile.h"



int typeWriter(char * filename){
    ofile * file = openFile(filename, 1, 2);
    rowData ** savedScreen = saveScreen();
    rowData ** screen = getScreen();
    configFile * config = getConfig();
    screenData * sd = getScreenData();
    char * twoptions = "  QUIT     SAVE     We really need more options here, this";
    char * shortcuts = " Ctrl+X   Ctrl+S    looks dumb af right now can't even lie";
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
    file->botrows[0]->color = config->twInstrColor;
    file->botrows[1]->color = config->twInstrColor;
    strcpy(file->botrows[0]->vals, twoptions);
    strcpy(file->botrows[1]->vals, shortcuts);
    printFile(file);
    sd->row = file->numtoprows;
    sd->col = 0;
    moveCursor(sd->row, sd->col);
    typingLoop(file);
    freeFile(file);
    sd->row = currow;
    loadScreen(savedScreen);
    return 0;
}


void typingLoop(ofile * file){
    screenData * sd = getScreenData();
    rowData ** screen = getScreen();
    rowData * row = file->rows[0];
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

        if(ch == 0x1B || ch == 0x18) return; // exit on ESC or Ctrl+X

        else if (ch == 0x13){ // Ctrl+S
            saveOfile(file);
            saved = 1;
        }

        else if (!ch && sc == 0x48){ // up arrow
            if (sd->row > file->numtoprows) {
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
            if (sd->row < ROWS - file->numbotrows - 1 && sd->row < file->numrows + file->numtoprows - 1){
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

        else if (ch){
            saved = 0;
            typeChar(file, ch);
        }

        // show modified or nothing depending on if saved
        i = 0;
        while (modified[i]){
            if (saved) file->toprows[0]->vals[COLS - strLen(modified) - 1 + i] = 0;
            else file->toprows[0]->vals[COLS - strLen(modified) - 1 + i] = modified[i];
            i++;
        }
        printFile(file);
        keyPress = 0;
    }
}


void printFile(ofile * file){
    int i, j, k;
    rowData ** screen = getScreen();
    rowData * filerow;
    rowData * row;
    int numrowstoprint = ROWS - file->numtoprows - file->numbotrows;
    if (file->startrow + file->numrows < numrowstoprint) numrowstoprint = file->startrow + file->numrows;
    for (i = 0; i < file->numtoprows; i++){
        row = screen[i];
        filerow = file->toprows[i];
        copyRow(row, filerow);
    }
    for (i = 0; i < numrowstoprint; i++){
        row = screen[i + file->numtoprows];
        filerow = file->rows[i + file->startrow];
        copyRow(row, filerow);
    }
    k = 0;
    for (i = ROWS - file->numbotrows; i < ROWS; i++){
        row = screen[i];
        filerow = file->botrows[k++];
        copyRow(row, filerow);
    }
    printScreen();
}


void copyRow(rowData * dest, rowData * src){
    int i;
    for (i = 0; i < COLS; i++) dest->vals[i] = src->vals[i];
    dest->newlineloc = src->newlineloc;
    dest->color = src->color;
}


void typeChar(ofile * file, char c){
    int i, j;
    screenData * sd = getScreenData();
    rowData * row = file->rows[sd->row - file->numtoprows + file->startrow];
    rowData * otherrow;

    if (c == 8) backspaceHandler(file);

    else if (c == 9){ // tab
        j = 4 - mod(sd->col, 4);
        for (i = 0; i < j; i++) typeChar(file, ' '); // tab aligns on 4 char intervals
    }

    else if (c == 13) // enter
        enterHandler(file);

    else{
        // what if line runs out of space for character
        i = row->newlineloc;
        while (i >= sd->col){
            row->vals[i + 1] = row->vals[i];
            i--;
        }
        row->vals[sd->col] = c;
        sd->col++;
        row->newlineloc++;
    }
    moveCursor(sd->row, sd->col);
}


void backspaceHandler(ofile * file){
    int i;
    screenData * sd = getScreenData();
    rowData * row = file->rows[sd->row - file->numtoprows + file->startrow];
    rowData ** newrows = (rowData**)malloc(sizeof(rowData*) * (file->numrows - 1));
    int currow = sd->row - file->numtoprows + file->startrow;
    if (sd->col > 0){
        i = sd->col - 1;
        while (row->vals[i]){
            row->vals[i] = row->vals[i + 1];
            i++;
        }
        sd->col--;
        row->newlineloc--;
    }
    else if (sd->row > file->numtoprows){ // doesnt work!
        if (row->newlineloc == 0){
            freeRowData(row);
            for (i = 0; i < file->numrows - 1; i++){
                if (i < currow - 1) newrows[i] = file->rows[i];
                else newrows[i] = file->rows[i + 1];
            }
            free(file->rows);
            file->rows = newrows;
            file->numrows--;
            sd->row--;
            sd->col = file->rows[sd->row - file->numtoprows + file->startrow]->newlineloc;
        }
    }
}


void enterHandler(ofile * file){
    int i, j;
    screenData * sd = getScreenData();
    rowData * row = file->rows[sd->row - file->numtoprows + file->startrow];
    rowData ** newrows = (rowData**)malloc(sizeof(rowData*) * (file->numrows + 1));
    int currow = sd->row - file->numtoprows + file->startrow;
    for (i = 0; i < file->numrows; i++){
        if (i < currow){
            newrows[i] = file->rows[i];
        }
        else if (i == currow){
            newrows[i] = file->rows[i];
            newrows[i + 1] = freshRow();
            for (j = sd->col; j < row->newlineloc; j++){
                newrows[i + 1]->vals[j - sd->col] = newrows[i]->vals[j];
                newrows[i]->vals[j] = 0;
            }
            newrows[i + 1]->newlineloc = newrows[i]->newlineloc - sd->col;
            newrows[i]->newlineloc = sd->col;
        }
        else{
            newrows[i + 1] = file->rows[i];
        }
    }
    if (sd->row < ROWS - file->numbotrows) sd->row++;
    else file->startrow++;
    sd->col = 0;
    free(file->rows);
    file->rows = newrows;
    file->numrows++;
}