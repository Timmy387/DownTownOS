#include "time.h"



fullTime * getTime(){
    fullTime * time = (fullTime*)malloc(sizeof(fullTime));
    unsigned short high;
    unsigned short low;
    short highbyte;
    short lowbyte;
    int milsPerTick = 55;
    time->hours = 0;
    time->minutes = 0;
    time->seconds = 0;
    time->milliseconds = 0;
    readTime(&high, &low);
    time->hours = high;
    highbyte = low >> 8;
    if (highbyte) highByteLoop(highbyte, time);

    lowbyte = low & 0xFF;
    lowbyte *= milsPerTick;
    time->seconds += lowbyte / 1000;
    lowbyte = mod(lowbyte, 1000);
    time->milliseconds += lowbyte;

    time->minutes = time->seconds / 60;
    time->seconds = mod(time->seconds, 60);
    return time;
}


void highByteLoop(short highbyte, fullTime * time){
    int milsPerOne256th = highbyte * 55;
    short i;
    for (i = 0; i < 256; i++){
        time->seconds += milsPerOne256th / 1000;
        time->milliseconds += mod(milsPerOne256th, 1000);
        time->seconds += time->milliseconds / 1000;
        time->milliseconds = mod(time->milliseconds, 1000);
    }
}


fullTime * timediff(fullTime * start, fullTime * end){
    fullTime * diff = (fullTime*)malloc(sizeof(fullTime));
    short temphrs = end->hours;
    short tempmins = end->minutes;
    short tempsecs = end->seconds;
    short tempmils = end->milliseconds;
    if (end->milliseconds >= start->milliseconds){
        diff->milliseconds = end->milliseconds - start->milliseconds;
    }
    else{
        diff->milliseconds = 1000 + end->milliseconds - start->milliseconds;
        if (end->seconds > 0) end->seconds--;
        else{
            end->seconds = 59;
            if (end->minutes > 0) end->minutes--;
            else{
                end->minutes = 59;
                if (end->hours > 0) end->hours--;
                else end->hours = 23;
            }
        }
    }

    if (end->seconds >= start->seconds) diff->seconds = end->seconds - start->seconds;
    else {
        diff->seconds = 60 + end->seconds - start->seconds;
        if (end->minutes > 0) end->minutes--;
        else{
            end->minutes = 59;
            if (end->hours > 0) end->hours--;
            else end->hours = 23;
        }
    }

    if (end->minutes >= start->minutes) diff->minutes = end->minutes - start->minutes;
    else {
        diff->minutes = 60 + end->minutes - start->minutes;
        if (end->hours > 0) end->hours--;
        else end->hours = 23;
    }

    if (end->hours >= start->hours) diff->hours = end->hours - start->hours;
    else{
        diff->hours = 24 + end->hours - start->hours;
    }

    end->hours = temphrs;
    end->minutes = tempmins;
    end->seconds = tempsecs;
    end->milliseconds = tempmils;
    return diff;
}


void delay(short ms, short secs, short mins, short hrs){
    fullTime * start = getTime();
    fullTime * end;
    fullTime * diff;
    while (1){
        end = getTime();
        diff = timediff(start, end);
        if (diff->hours > hrs) break;
        else if (diff->hours == hrs && diff->minutes > mins) break;
        else if (diff->hours == hrs && diff->minutes == mins && diff->seconds > secs) break;
        else if (diff->hours == hrs && diff->minutes == mins && diff->seconds == secs && diff->milliseconds >= ms) break;
        free(diff);
        free(end);
    }
    free(diff);
    free(end);
    free(start);
    return;
}