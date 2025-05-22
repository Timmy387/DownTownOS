#include "../assembly/asm.h"
#include "handler.h"
#include "../utils/util.h"
#include "../strings/pStr.h"
#include "../strings/string.h"
#include "commands.h"
#include "../memory/malloc.h"

#define WHITE_ON_BLACK 0x0F


void commandGetter(){
    short keyPress = 0;
    char ch = 0;
    char sc = 0;
    short countsincespace = 0;
    String * command = newString("");
    String * prompt = newString("$ ");
    String * lastCommand = newString("");
    print(prompt);

    while (1){
        while (keyPress == 0) keyPress = inb(0);
        ch = keyPress & 0xFF;
        sc = keyPress >> 8;
        if (ch == 13){
            freeString(lastCommand);
            lastCommand = newString(command->s);
            commandHandler(command);
            freeString(command);
            command = newString("");
            print(prompt);
        }
        else if (ch == 12){ // Ctrl+L
            freeString(command);
            command = newString("clear");
            commandHandler(command);
            freeString(command);
            command = newString("");
            print(prompt);
        }
        else if (ch == 3){ // Ctrl+C
            freeString(lastCommand);
            lastCommand = newString(command->s);
            printLiteral("\n\n");
            freeString(command);
            command = newString("");
            print(prompt);
        }
        else if (ch == 18) // Ctrl+R
            asm("int 0x19"); // reset
        else if (!ch && sc == 0x48){
            clearCommand(command);
            freeString(command);
            command = newString(lastCommand->s);
            print(command);
        }
        else {
            if (ch == ' ') countsincespace = 0;
            else if (ch == 8 && countsincespace) countsincespace--;
            else if (countsincespace < 32) countsincespace++;
            if (countsincespace < 32){
                if (ch != 8 || command->len) printChar(ch);
                addChar(command, ch);
            }
        }
        keyPress = 0;
    }
}


void commandHandler(String * fullStr){
    String * command;
    int i;
    int numOptions = 0;
    String ** arguments = split(fullStr, ' ');
    while (arguments[numOptions]) numOptions++;
    command = arguments[0];
    printChar('\n');
    if (!command);
    else if (strEqual(command->s, "typewriter") || strEqual(command->s, "tw")) typeWriterCmd(arguments, numOptions);
    else if (strEqual(command->s, "bounce")) bounce(arguments, numOptions);
    else if (strEqual(command->s, "hop")) hop(arguments, numOptions);
    else if (strEqual(command->s, "pwd")) pwdCmd(arguments, numOptions);
    else if (strEqual(command->s, "ls")) lsCmd(arguments, numOptions);
    else if (strEqual(command->s, "cd")) cdCmd(arguments, numOptions);
    else if (strEqual(command->s, "mkdir")) mkdirCmd(arguments, numOptions);
    else if (strEqual(command->s, "touch")) touchCmd(arguments, numOptions);
    else if (strEqual(command->s, "rm")) rmCmd(arguments, numOptions);
    else if (strEqual(command->s, "echo")) echo(arguments, numOptions);
    else if (strEqual(command->s, "color")) changeColor(arguments, numOptions);
    else if (strEqual(command->s, "clear")) clear(arguments, numOptions);
    else if (strEqual(command->s, "help")) help(arguments, numOptions);
    else if (strEqual(command->s, "halt")) halt();
    else if (strEqual(command->s, "reset")) reset(arguments, numOptions);
    else error(command);

    // prepare for next command

    if (!strEqual(command->s, "clear") || (numOptions > 1 && strEqual(arguments[1]->s, "-h")))
        printLiteral("\n");
    i = 0;
    while (arguments[i]){
        freeString(arguments[i]);
        i++;
    }
    free(arguments);
    return;
}


void clearCommand(String * cmd){
    int i;
    char * backspaces = (char*)malloc(cmd->len + 1);
    for (i = 0; i < cmd->len; i++){
        backspaces[i] = 8;
    }
    backspaces[i] = 0;
    printLiteral(backspaces);
    free(backspaces);
}