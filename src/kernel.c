#include "assembly/asm.h"
#include "memory/malloc.h"
#include "utils/screen.h"
#include "commands/handler.h"
#include "strings/string.h"
#include "files/fileBackend.h"

void welcome();

int main(void) {
    initVideo();
    initHeap();
    initFileSystem();
    moveToHome();
    moveCursor(ROWS - 1, 0);
    scroll(ROWS);
    welcome();
    commandGetter();
}


void welcome(){
    printLiteral("Welcome to Dave and Timmy's OS, DownTownOS.\n");
    printLiteral("Type help for a list of commands you can use.\n");
}