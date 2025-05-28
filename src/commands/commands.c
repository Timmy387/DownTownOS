#include "commands.h"
#include "../utils/util.h"
#include "../strings/pStr.h"
#include "../strings/string.h"
#include "../assembly/asm.h"
#include "../utils/screen.h"
#include "../files/fileCmds.h"
#include "../files/fileBackend.h"
#include "../textEditor/typeWriter.h"

#define WHITE_ON_BLACK 0x0F


void typeWriterCmd(String ** arguments, int numOptions){
    String * filename = arguments[1];
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("typewriter/tw <filename>: edit file in our custom text editor!\n If specified \
file doesn't exist it will be created then opened.\n");
        return;
    }
    if (!fileExist(filename->s)) touch(filename->s);
    if (!filename) return;
    typeWriter(filename->s);
}


// void hop(String ** arguments, int numOptions){
//     char first;
//     char * arg;
//     if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
//         printLiteral("hop <string> <speed>: makes given string hop across screen\nat speed 1(slow), 2(medium), or 3(fast).\n");
//         printLiteral("Press ESC to return to terminal.\n");
//         return;
//     }
//     if (numOptions == 1) hopAround("Hello, World!", 2);
//     if (numOptions == 2){
//         arg = arguments[1]->s;
//         first = arg[0];
//         if (first >= '0' && first <= '9') hopAround("Hello, World!", atoi(arg));
//         else hopAround(arg, 2);
//     }
//     if (numOptions > 2) hopAround(arguments[1]->s, atoi(arguments[2]->s));
// }


void pwdCmd(String ** arguments, int numOptions){
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("pwd: print working directory.\n");
        return;
    }
    pwd();
}


void lsCmd(String ** arguments, int numOptions){
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("ls: lists all files and directories within the current directory.\n");
        return;
    }
    ls();
}


void cdCmd(String ** arguments, int numOptions){
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("cd <directory>: changes current working directory to specified directory.\ncd <..>: moves up one directory.\n");
        return;
    }
    if (numOptions == 1) cd("/");
    if (numOptions > 1) cd(arguments[1]->s);
}


void mkdirCmd(String ** arguments, int numOptions){
    String * dirname = arguments[1];
    int errorNum;
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("mkdir <directoryname>: creates a new directory within the current directory.\n");
        return;
    }
    if (!dirname) return;
    if (dirname->len > 31) {
        dirname->s[31] = 0;
        dirname->len = 31;
    }
    if (dirname->len == 31){
        if (dirname->s[30] != '/'){
            dirname->s[30] = '/';
        }
    }
    else{
        if (dirname->s[dirname->len - 1] != '/'){
            dirname->s[dirname->len] = '/';
            dirname->s[dirname->len + 1] = 0;
        }
    }
    errorNum = mkdir(dirname->s);
    if (errorNum > 0){
        if (errorNum == 1){
            printLiteral("Error: file or directory <");
            print(dirname);
            printLiteral("> already exists\n");
            return;
        }
        else if (errorNum == 2){
            printLiteral("Error: how the fuck did you use all 720k space on this tiny ahh computer?\n");
            return;
        }
        else if (errorNum == 3){
            printLiteral("Error: disk write failed\n");
            return;
        }
    }
}


void touchCmd(String ** arguments, int numOptions){
    String * filename = arguments[1];
    int errorNum;
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("touch <filename>: creates an empty text file with the given name.\n");
        return;
    }
    if (!filename) {
        printLiteral("Enter a filename to create\n");
        return;
    }
    filename->s[MAX_NAME_LEN] = 0;
    errorNum = touch(filename->s);
    if (errorNum > 0){
        if (errorNum == 1){
            printLiteral("Error: file or directory <");
            print(filename);
            printLiteral("> already exists\n");
            return;
        }
        else if (errorNum == 2){
            printLiteral("Error: how the fuck did you use all 720k space on this tiny ahh computer?\n");
            return;
        }
        else if (errorNum == 3){
            printLiteral("Error: disk write failed\n");
            return;
        }
    }
}


void echo(String ** arguments, int numOptions){
    
}


void rmCmd(String ** arguments, int numOptions){
    int rows = 1;
    String * filename = arguments[1];
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("rm <filename>: deletes file with given name.\n");
        printLiteral("rm <directoryname>: recursively deletes everything in given\n directory including itself.\n");
        return;
    }
    if (!filename) {
        printLiteral("Enter a file name to be deleted\n");
        return;
    }
    if (filename->s[0] == '/'){
        printLiteral("Bro why you trying to delete the root directory??\nChill, you need that shit.\n");
        return;
    }
    if (!rm(filename->s)){
        printLiteral("Error: file not found\n");
    }
}


void changeColor(String ** arguments, short numOptions){
    String * newColor;
    configFile * config = getConfig();
    rowData ** screen = getScreen();
    short color;
    short bg = 0;
    int i = 1;
    char * temp;
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("color <color>: changes text color.\n");
        printLiteral(" -bg flag: changes background color instead.\n");
        printLiteral(" -l flag: lists available colors. Background can only be set to the first 8.\n");
        return;
    }
    else if (numOptions > 1 && strEqual(arguments[1]->s, "-l")){
        printLiteral(" black, blue, green, cyan,\n red, magenta, brown, lightgray,\n");
        printLiteral(" darkgray, lightblue, lightgreen, lightcyan\n lightred, pink, yellow, white\n");
        return;
    }
    if (!newColor) {
        printLiteral("Enter a color");
        return;
    }

    while (arguments[i]) {
        if (strEqual(arguments[i]->s, "-bg")) {
            bg++;
            if (i == 1) newColor = arguments[2];
            if (i == 2) newColor = arguments[1];
            toLower(newColor->s);
            break;
        }
        i++;
    }

    if (!bg){
        newColor = arguments[1];
        toLower(newColor->s);
        color = config->color;
        config->color = config->color & 0xF0;
        if (strEqual(newColor->s, "black"))             config->color = config->color | 0x00;
        else if (strEqual(newColor->s, "blue"))         config->color = config->color | 0x01;
        else if (strEqual(newColor->s, "green"))        config->color = config->color | 0x02;
        else if (strEqual(newColor->s, "cyan"))         config->color = config->color | 0x03;
        else if (strEqual(newColor->s, "red"))          config->color = config->color | 0x04;
        else if (strEqual(newColor->s, "magenta"))      config->color = config->color | 0x05;
        else if (strEqual(newColor->s, "brown"))        config->color = config->color | 0x06;
        else if (strEqual(newColor->s, "lightgray"))    config->color = config->color | 0x07;
        else if (strEqual(newColor->s, "darkgray"))     config->color = config->color | 0x08;
        else if (strEqual(newColor->s, "lightblue"))    config->color = config->color | 0x09;
        else if (strEqual(newColor->s, "lightgreen"))   config->color = config->color | 0x0A;
        else if (strEqual(newColor->s, "lightcyan"))    config->color = config->color | 0x0B;
        else if (strEqual(newColor->s, "lightred"))     config->color = config->color | 0x0C;
        else if (strEqual(newColor->s, "pink"))         config->color = config->color | 0x0D;
        else if (strEqual(newColor->s, "yellow"))       config->color = config->color | 0x0E;
        else if (strEqual(newColor->s, "white"))        config->color = config->color | 0x0F;
        else{
            config->color = color;
            printLiteral("color not recognized: ");
            print(newColor);
            printLiteral("\n");
            printScreen();
            return;
        }
    }
    else{
        config->color = config->color & 0x0F;
        color = config->color;
        if (strEqual(newColor->s, "black"))             config->color = config->color | 0x00;
        else if (strEqual(newColor->s, "blue"))         config->color = config->color | 0x10;
        else if (strEqual(newColor->s, "green"))        config->color = config->color | 0x20;
        else if (strEqual(newColor->s, "cyan"))         config->color = config->color | 0x30;
        else if (strEqual(newColor->s, "red"))          config->color = config->color | 0x40;
        else if (strEqual(newColor->s, "magenta"))      config->color = config->color | 0x50;
        else if (strEqual(newColor->s, "brown"))        config->color = config->color | 0x60;
        else if (strEqual(newColor->s, "lightgray"))    config->color = config->color | 0x70;
        else{
            config->color = color;
            printLiteral("color not recognized: ");
            print(newColor);
            printLiteral("\n");
            printScreen();
            return;
        }
    }
    for (i = 0; i < ROWS; i++){
        screen[i]->color = config->color;
    }
    saveConfig();
    printScreen();
}


void clear(String ** arguments, short numOptions){
    screenData * sd = getScreenData();
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("clear: clears screen.\n");
        return;
    }
    scroll(sd->row);
}


void help(String ** arguments, short numOptions){
    printLiteral("Command options include:\n  help(duh)\n  color\n  clear (Ctrl+L)\n  reset (Ctrl+R)\n");
    printLiteral("  mkdir\n  rm\n  ls\n  cd\n  pwd\n  touch\n  typewriter/tw\n");
    printLiteral(" And any others you add!\n Add the -h flag to a command to learn more about it.\n");
}


void halt(){
    resetScreen();
    loop:
    asm("hlt");
    goto loop;
}


void reset(String ** arguments, short numOptions){
    if (numOptions > 1 && strEqual(arguments[1]->s, "-h")){
        printLiteral("reset: reboots machine.\n");
        return;
    }
    asm("int 0x19");
}


void error(String * command){
    print(command);
    printLiteral(" is not recognized as a command.\n");
}