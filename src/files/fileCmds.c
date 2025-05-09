#include "fileCmds.h"
#include "fileBackend.h"
#include "../utils/util.h"
#include "../strings/string.h"


// functions for file system
void moveToHome(){
    configFile * config = getConfig();
    if (!config->initFiles){
        config->initFiles = 1;
        saveConfig();
        // writeFile((inode*)config, "something");
        mkdir("users/");
        cd("users/");
        mkdir("timmy/");
        mkdir("dave/");
        cd("timmy/");
    }
    else{
        cd("users/");
        cd("timmy/");
    }
}

/*
permenantly delete a file or directory by clearing memory
*/
short rm(char * filename){
    inode * currDir = getCurrDir();
    inode * file;
    int i;
    int num;
    for (i = 0; i < DIRECTS_SIZE; i++){
        num = currDir->directs[i];
        if (num){
            file = readFromDiskC(num);
            // need this to recursively delete everything else too!! (find file, loop through it, 
            // delete everything, loop through subdirectories too)
            if (strEqual(file->filename, filename)){
                currDir->directs[i] = 0;
                freeBitmask(currDir->directs[i]);
                writeToDiskC(currDir);
                return 1;
            }
            freeInode(file);
        }
    }
    return 0;
}


/*
make directory
*/
int mkdir(char * name){
    char * bitmask = getBitmask();
    inode * currDir = getCurrDir();
    int freeBlock;
    int i = 0;
    int success;
    inode * newDir = (inode*)malloc(BLOCK_SIZE);
    if (fileExist(name)) {
        freeInode(newDir);
        return 1;
    }
    freeBlock = findFreeBlock();
    if (!freeBlock) {
        freeInode(newDir);
        return 2;
    }
    for (i = 0; i < BLOCK_SIZE; i++) ((char*)newDir)[i] = 0;
    newDir->bitmask_num = freeBlock;
    newDir->dir = 1;
    strcpy(((char*)newDir) + sizeof(inode), name);
    newDir->file_size = 1;
    newDir->filename = 0;
    newDir->parentNum = currDir->bitmask_num;
    success = writeToDiskC(newDir);
    // now add freeBlock to the current directory's direct list
    i = 0;
    while (currDir->directs[i]) i++;
    currDir->directs[i] = freeBlock;
    writeToDiskC(currDir);
    freeInode(newDir);
    if (success) return 0;
    return 3;
}  // mkdir


void ls(){
    int i;
    inode * file;
    inode * currDir = getCurrDir();
    for (i = 0; i < DIRECTS_SIZE; i++){
        if (currDir->directs[i]){
            file = readFromDiskC(currDir->directs[i]);
            printLiteral(file->filename);
            printLiteral("\n");
            freeInode(file);
        }
    }
}


int cd(char * dirname){
    inode * currDir = getCurrDir();
    inode * file;
    inode * newDir;
    int i;
    if (dirname[0] == '/'){
        // currDir->bitmask_num = 1;
        changeCurrDir(1);
        // newDir = readFromDiskC(1);
        // getCurrDir(newDir);
        return 1;
    }
    else if (strEqual(dirname, "..")){
        if (!strEqual(currDir->filename, "/")){
            changeCurrDir(currDir->parentNum);
            // newDir = readFromDiskC(currDir->parentNum);
            // getCurrDir(newDir);
            return 0;
        }
        else {
            printLiteral("You're already at the root ya goofball\n");
            return 1;
        }
    }
    else{
        for (i = 0; i < DIRECTS_SIZE; i++){
            if (currDir->directs[i]){
                newDir = readFromDiskC(currDir->directs[i]);
                if (strEqual(newDir->filename, dirname) && file->dir){
                    if (file->dir){
                        changeCurrDir(newDir->bitmask_num);
                        freeInode(newDir);
                        // getCurrDir(newDir);
                        return 1;
                    }
                    else{
                        printLiteral("Error: not a directory: ");
                        printLiteral(dirname);
                        printLiteral("\n");
                        freeInode(newDir);
                        return 0;
                    }
                }
                freeInode(newDir);
            }
        }
        printLiteral("Error: directory not found: ");
        printLiteral(dirname);
        printLiteral("\n");
        return 0;
    }
    return 0;
}


void pwd(){
    inode * currDir = getCurrDir();
    inode * dir = 0;
    String * fullPath = newString(currDir->filename);
    String * tempStr = fullPath;
    if (currDir->parentNum){ // if not root dir
        dir = readFromDiskC(currDir->parentNum);
        fullPath = newString(dir->filename);
        concat(fullPath, tempStr->s);
        freeString(tempStr);
        tempStr = fullPath;
    }
    while (dir && dir->parentNum){
        freeInode(dir);
        dir = readFromDiskC(dir->parentNum);
        fullPath = newString(dir->filename);
        concat(fullPath, tempStr->s);
        freeString(tempStr);
        tempStr = fullPath;
    }
    freeInode(dir);
    print(fullPath);
    printLiteral("\n");
    freeString(fullPath);
}


/*
make file (allocates memory)
*/
int touch(char* name){
    char * bitmask = getBitmask();
    inode * currDir = getCurrDir();
    int freeBlock;
    int i = 0;
    int success;
    inode * newFile = (inode*)malloc(BLOCK_SIZE);
    if (fileExist(name)) {
        freeInode(newFile);
        return 1;
    }
    freeBlock = findFreeBlock();
    if (!freeBlock) {
        freeInode(newFile);
        return 2;
    }
    for (i = 0; i < BLOCK_SIZE; i++) ((char*)newFile)[i] = 0;
    newFile->bitmask_num = freeBlock;
    newFile->dir = 0;
    strcpy(((char*)newFile) + sizeof(inode), name);
    newFile->file_size = 1;
    newFile->filename = 0;
    newFile->parentNum = currDir->bitmask_num;
    success = writeToDiskC(newFile);
    // now add freeBlock to the current directory's direct list
    i = 0;
    while (currDir->directs[i]) i++;
    currDir->directs[i] = freeBlock;
    writeToDiskC(currDir);
    freeInode(newFile);
    if (success) return 0;
    return 3;
}  // touch


// maybe an ls command or is that kernel?