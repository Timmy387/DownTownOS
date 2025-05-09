#ifndef TIME_H
#define TIME_H

typedef struct {
    short hours;
    short minutes;
    short seconds;
    short milliseconds;
} fullTime;

fullTime * getTime();

void highByteLoop(int highbyte, fullTime * time);

fullTime * timediff(fullTime * start, fullTime * end);

void delay(short ms, short secs, short mins, short hrs);

#endif