#include "fileBackend.h"
#include "../utils/util.h"

/*
initialize file system
*/
void initFileSystem(){
    char * firstBlock = (char*)malloc(BLOCK_SIZE);
    char * bitmask = getBitmask();
    configFile * config = getConfig();
    inode * rootdir = (inode*)malloc(BLOCK_SIZE);
    int i;
    readFromDisk(firstBlock, FILE_START_HEAD, FILE_START_TRACK, BITMASK_SECTOR);
    if (!config->init){
        buildConfig(config);
        saveConfig(config);
        // build bitmask
        bitmask[0] = 1;
        bitmask[1] = 0x07;
        for (i = 2; i < BLOCK_SIZE; i++) bitmask[i] = 0;
        for (i = 0; i < BLOCK_SIZE; i++) ((char*)rootdir)[i] = 0;
        // build rootdir
        rootdir->bitmask_num = 1;
        rootdir->parentNum = 0;
        rootdir->dir = 1;
        rootdir->file_size = 1;
        rootdir->filename = 0;
        rootdir->directs = 0;
        ((char*)rootdir)[sizeof(inode)] = '/';
        ((char*)rootdir)[sizeof(inode) + 1] = 0;
        writeToDisk(bitmask, FILE_START_HEAD, FILE_START_TRACK, BITMASK_SECTOR);
        writeToDisk(rootdir, FILE_START_HEAD, FILE_START_TRACK, ROOT_DIR_SECTOR);
    }
    free(firstBlock);
    free(rootdir);
}


void buildConfig(configFile * config){
    config->init = 1;
    config->color = 0x0F;
    writeToDisk(config, FILE_START_HEAD, FILE_START_TRACK, CONFIG_SECTOR);
}


void saveConfig(){
    configFile * config = getConfig();
    writeToDisk(config, FILE_START_HEAD, FILE_START_TRACK, CONFIG_SECTOR);
}



configFile * getConfig(){
    static short init = 0;
    static configFile * config;
    if (!init){
        init = 1;
        config = (configFile*)malloc(BLOCK_SIZE);
        readFromDisk(config, FILE_START_HEAD, FILE_START_TRACK, CONFIG_SECTOR);
    }
    return config;
}


void freeInode(inode * file){
    free(file);
}


char * getBitmask(){
    static int init = 0;
    static char * bitmask;
    if (!init){
        init = 1;
        bitmask = (char*)malloc(BLOCK_SIZE);
        readFromDisk(bitmask, FILE_START_HEAD, FILE_START_TRACK, BITMASK_SECTOR);
    }
    return bitmask;
}


short bitStatus(short index){
    char * bitmask = getBitmask();
    short byte = index / 8 + 1;
    short bit = mod(index, 8);
    bit = 1 << bit;
    return bitmask[byte] & bit;
}


inode * getCurrDir(){
    static inode * current;
    static short initialized = 0;
    if (!initialized){
        initialized++;
        current = readFromDiskC(1); // start at root dir
    }
    return current;
}


void changeCurrDir(short bitmaskNum){
    int i;
    inode * currDir = getCurrDir();
    inode * newDir = readFromDiskC(bitmaskNum);
    newDir->filename = currDir->filename;
    newDir->directs = currDir->directs;
    for (i = 0; i < BLOCK_SIZE; i++){
        ((unsigned char*)currDir)[i] = ((unsigned char*)newDir)[i];
    }
    freeInode(newDir);
}


/*
reads in a file from a given block index.
MUST FREE after use, allocates memory dynamically and does not free.
*/
inode * readFromDiskC(short index){
    inode * file = (inode*)malloc(BLOCK_SIZE);
    short head = 0;
    short track = 0;
    short sector = 0;
    short rem = 0;
    track = index / 18;
    track += FILE_START_TRACK;
    rem = mod(index, 18);
    head = rem / 9;
    sector = mod(rem, 9) + 1;
    readFromDisk(file, head, track, sector);
    file->filename = ((unsigned char*)file) + sizeof(inode);
    file->directs = (short*)(((unsigned char*)file) + sizeof(inode) + MAX_NAME_LEN);
    return file;
}


int writeToDiskC(inode * file){
    short head = 0;
    short track = 0;
    short sector = 0;
    short rem = 0;
    track = file->bitmask_num / 18;
    track += FILE_START_TRACK;
    rem = mod(file->bitmask_num, 18);
    head = rem / 9;
    sector = mod(rem, 9) + 1;
    useBitmask(file->bitmask_num);
    return writeToDisk(file, head, track, sector);
}


int fileExist(char * filename){
    inode * curDir = getCurrDir();
    inode * tempfile;
    int i = 0;
    int dir = 0;
    if (filename[strLen(filename) - 1] == '/') dir++;
    for(i = 0; i < DIRECTS_SIZE; i++){
        if(curDir->directs[i]){
            tempfile = readFromDiskC(curDir->directs[i]);
            if ((dir && tempfile->dir) || !(dir || tempfile->dir)){
                if (strEqual(filename, tempfile->filename)){
                    freeInode(tempfile);
                    return 1;
                }
            }
            else if (tempfile->dir && !dir){
                tempfile->filename[strLen(tempfile->filename) - 1] = 0;
                if (strEqual(filename, tempfile->filename)){
                    freeInode(tempfile);
                    return 1;
                }
            }
            else if (!tempfile->dir && dir){
                filename[strLen(filename) - 1] = 0;
                if (strEqual(filename, tempfile->filename)){
                    freeInode(tempfile);
                    return 1;
                }
            }
            freeInode(tempfile);
        }
    }
    return 0;
} // fileExist


/*
find the first available free block 
*/
int findFreeBlock(){
    char * bitmask = getBitmask();
    int i;
    for (i = 2; i < MAX_BLOCKS; i++){
        if (!bitStatus(i)){
            return i;
        }
    }
    return 0;
}


/*
set block at provided index as used in bitmask
*/
short useBitmask(short index){
    char * bitmask = getBitmask();
    short byte = index / 8 + 1;
    short bit = mod(index, 8);
    bit = 1 << bit;
    if (!bitStatus(index)) {
        bitmask[byte] = bitmask[byte] | bit;
        writeToDisk(bitmask, FILE_START_HEAD, FILE_START_TRACK, BITMASK_SECTOR);
        return 1;
    }
    else {
        return 0;
    }
}

/*
set block at provided index as free in bitmask
*/
short freeBitmask(short index){
    char * bitmask = getBitmask();
    short byte = index / 8 + 1;
    short bit = mod(index, 8);
    bit = 1 << bit;
    bit = bit ^ 0xFF;
    if (bitStatus(index)) {
        bitmask[byte] = bitmask[byte] & bit;
        writeToDisk(bitmask, FILE_START_HEAD, FILE_START_TRACK, BITMASK_SECTOR);
        return 1;
    }
    else {
        return 0;
    }
}


/*
find inode with same name, return inode or 0 if not found
MUST FREE after use, returns a malloced pointer to an inode.
*/
inode * findInode(char* filename){
    // loop thru free memory bitmask
    char * bitmask = getBitmask();
    int i;
    inode * file = (inode*)malloc(BLOCK_SIZE);
    char * readInName = (char*)malloc(32);
    short head;
    short track;
    short sector;
    int rem;
    for (i = 1; i < MAX_BLOCKS; i++){
        if (bitStatus(i)){
            track = i / 18;
            rem = mod(i, 18);
            head = rem / 9;
            sector = mod(rem, 9) + 1;
            readFromDisk(file, head, track + FILE_START_TRACK, sector);
            strcpy(readInName, ((char*)file) + sizeof(inode));
            if (strEqual(readInName, filename)) {
                free(readInName);
                return file;
            }
        }
    }
    free(file);
    free(readInName);
    return 0;
}

/*
contents MUST be freed later
*/
char * readFileContents(char * filename){
    inode * currDir = getCurrDir();
    inode * file;
    char * contents = (char*)malloc(4 * BLOCK_SIZE); // not dynamic, but can't be yet
    int i;
    for (i = 0; i < 4 * BLOCK_SIZE; i++){
        contents[i] = ' ';
    }
    if (!fileExist(filename)){
        return 0;
    }
    for (i = 0; i < DIRECTS_SIZE; i++){
        if (currDir->directs[i]){
            file = readFromDiskC(currDir->directs[i]);
            if (strEqual(file->filename, filename)){
                if (file->dir){
                    freeInode(file);
                    return 0;
                }
                readHelper(file, contents);
                freeInode(file);
                contents[4 * BLOCK_SIZE - 1] = 0;
                return contents;
            }
            freeInode(file);
        }
    }
    return 0;
}


void readHelper(inode * file, char * contents){
    int i;
    int index;
    for (i = 0; i < DIRECTS_SIZE; i++){
        index = file->directs[i];
        if (index){
            short head = 0;
            short track = 0;
            short sector = 0;
            short rem = 0;
            track = index / 18;
            track += FILE_START_TRACK;
            rem = mod(index, 18);
            head = rem / 9;
            sector = mod(rem, 9) + 1;
            readFromDisk(contents, head, track, sector);
            contents += BLOCK_SIZE;
        }
    }
}


int saveFile(char * filename, char * contents){
    inode * currDir = getCurrDir();
    inode * file;
    int size = strLen(contents);
    char * writeStr;
    int i;
    int j;
    int freeBlock;
    size = size / BLOCK_SIZE + 1;
    writeStr = (char*)malloc(size * BLOCK_SIZE);
    strcpy(writeStr, contents);
    if (!fileExist(filename)){
        return 0;
    }
    for (i = 0; i < DIRECTS_SIZE; i++){
        if (currDir->directs[i]){
            file = readFromDiskC(currDir->directs[i]);
            if (strEqual(file->filename, filename)){
                if (file->dir){
                    freeInode(file);
                    return 0;
                }
                for (j = 0; j < DIRECTS_SIZE; j++){
                    freeBitmask(file->directs[j]);
                    file->directs[j] = 0;
                }
                for (j = 0; j < size; j++){
                    if (!file->directs[j] && size > 0){
                        freeBlock = findFreeBlock();
                        writeFile(contents, freeBlock);
                        file->directs[j] = freeBlock;
                        contents += BLOCK_SIZE;
                    }
                }
                writeToDiskC(file);
                freeInode(file);
                return 0;
            }
            freeInode(file);
        }
    }
    return 0;
}


int writeFile(char * contents, int index){
    short head = 0;
    short track = 0;
    short sector = 0;
    short rem = 0;
    track = index / 18;
    track += FILE_START_TRACK;
    rem = mod(index, 18);
    head = rem / 9;
    sector = mod(rem, 9) + 1;
    useBitmask(index);
    writeToDisk(contents, head, track, sector);
}