#include "typeWriter.h"
#include "../files/fileBackend.h"
#include "../strings/string.h"
#include "../utils/screen.h"

int typeWriter(char * filename){
    char * contents;
    char * screenBuff = saveScreen();
    screenData * sd = getScreenData();
    int currow = sd->row;
    String * file;
    int i;
    char c;
    if (!fileExist(filename)) {
        loadScreen(screenBuff);
        return 1;
    }
    contents = readFileContents(filename);
    if (!contents){
        loadScreen(screenBuff);
        return 2;
    }
    file = newString(contents);
    free(contents);
    file->s[SCREENSIZE] = 0;
    // printLiteral("Press ESC to quit");
    sd->row = 0;
    sd->col = 0;
    moveCursor(sd->row, sd->col);
    print(file);
    sd->row = 0;
    sd->col = 0;
    moveCursor(sd->row, sd->col);
    typingLoop(filename);
    sd->row = currow;
    freeString(file);
    loadScreen(screenBuff);
}



void typingLoop(char * filename){
    screenData * sd = getScreenData();
    short keyPress = 0;
    char ch = 0;
    int saved = 0;
    char sc = 0;
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
                saveFile(filename, getScreen());
                saved = 1;
            }
        }
        else if (!ch && sc == 0x48){ // up arrow
            if (sd->row) sd->row--;
            else sd->col = 0;
            moveCursor(sd->row, sd->col);
        }
        else if (!ch && sc == 0x4B){ // left arrow
            if (sd->col) sd->col--;
            else if (sd->row){
                sd->row--;
                sd->col = 79;
            }
            moveCursor(sd->row, sd->col);
        }
        else if (!ch && sc == 0x50){ // down arrow
            if (sd->row < ROWS - 1) sd->row++;
            else sd->col = 79;
            moveCursor(sd->row, sd->col);
        }
        else if (!ch && sc == 0x4D){ // right arrow
            if (sd->col < COLS - 1) sd->col++;
            else if (sd->row < ROWS - 1){
                sd->row++;
                sd->col = 0;
            }
            moveCursor(sd->row, sd->col);
        }
        else if(ch == 0x1B) return;
        else  if (ch){
            saved = 0;
            printChar(ch);
        }
        keyPress = 0;
    }
}