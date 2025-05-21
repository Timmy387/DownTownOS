#ifndef FILEBACKEND_H
#define FILEBACKEND_H

#define BLOCK_SIZE 512
#define FILE_START_HEAD 0
#define FILE_START_TRACK 7
#define CONFIG_SECTOR 3
#define BITMASK_SECTOR 1
#define ROOT_DIR_SECTOR 2
#define MAX_BLOCKS 1024
#define MAX_NAME_LEN 32
#define DIRECTS_SIZE sizeof(inode) + MAX_NAME_LEN - sizeof(short) // one space for the indirect

typedef struct{
    int inode_ptr[BLOCK_SIZE / sizeof(int)];
} indirect_1;

typedef struct{
    int bitmask_num;        // inode number in bitmask
    int parentNum;          // bitmask num of parent directory
    int dir;                // binary directory indicator
    int file_size;          // file size
    int saved;
    char * filename;
    short * directs;
} inode;

typedef struct {
    short init;
    short initFiles;
    short color;
    short twTitleColor;
} configFile;

void initFileSystem();

void buildConfig(configFile * config);

void saveConfig();

configFile * getConfig();

inode * getCurrDir();

void changeCurrDir(short bitmaskNum);

void freeInode(inode * file);

char * getBitmask();

short useBitmask(short index);

short freeBitmask(short index);

short bitStatus(short index);

inode * readFromDiskC(short index);

int writeToDiskC(inode * file);

int fileExist(char * filename);

int findFreeBlock();

inode * findInode(char* filename);

char * readFileContents(char * filename);

void readHelper(inode * file, char * contents);

int saveFile(char * filename, char * contents);

int writeFile(char * contents, int index);

#endif