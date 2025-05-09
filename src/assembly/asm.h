#ifdef ASM_H
#define ASM_H

void initVideo(void);
void putInMemory(int segment, int offset, char b);
char inb(void);
void moveCursor(int row, int col);
void hideCursor(void);
void readInScreen(unsigned short * buff);
short getCursorPos(void);
char getCursorRow(void);
void readTime(unsigned short * high, unsigned short * low);
void setTime(short hours, short minutes, short seconds);
void writeToDisk(inode * target, short head, short track, short sector);
void readFromDisk(inode * target, short head, short track, short sector);
void charToMouse(char c, char color);

#endif