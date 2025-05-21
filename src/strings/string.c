#include "string.h"
#include "../utils/util.h"
#include "../assembly/asm.h"
#include "../utils/screen.h"

String * newString(char * s){
    int slen = strLen(s);
    String * new = (String*)malloc(sizeof(String));
    char * str = (char*)malloc(slen + 2);
    strcpy(str, s);
    new->s = str;
    new->len = slen;
    return new;
}

void freeString(String * s){
    free(s->s);
    free(s);
}


void addChar(String * s, char c){
    char * temp;
    if (c == 8){
        if (s->len){
            s->s[s->len - 1] = 0;
            s->len--;
        }
        return;
    }
    temp = (char*)malloc(s->len + 2);
    strcpy(temp, s->s);
    temp[s->len] = c;
    s->len++;
    temp[s->len] = 0;
    free(s->s);
    s->s = temp;
}


void print(String * str){
    screenData * sd = getScreenData();
    rowData ** screen = getScreen();
    rowData * row = screen[sd->row];
    char c;
    int i, j;
    for (i = 0; i < str->len; i++){
        c = str->s[i];
        // backspace
        if (c == 8){
            if (sd->col == 0){
                if (sd->row == 0) continue;
                row = screen[sd->row];
                sd->col = row->newlineloc;
                sd->row--;
            }
            sd->col--;
            row->newlineloc--;
            row->vals[row->newlineloc] = 0;
        }
        else {
            if (row->newlineloc == COLS || c == '\n'){
                sd->row++;
                sd->col = 0;
                if (sd->row == ROWS){
                    scroll(1);
                }
                row = screen[sd->row];
            }
            if (c != '\n'){
                row->vals[row->newlineloc] = c;
                row->vals[row->newlineloc + 1] = 0;
                row->newlineloc++;
                sd->col++;
            }
        }
    }
    moveCursor(sd->row, sd->col);
    printScreen();
}


int printChar(char c){
    String * str = newString(" ");
    str->s[0] = c;
    print(str);
    freeString(str);
}



int printLiteral(char * str){
    String * s = newString(str);
    print(s);
    freeString(s);
}


void concat(String * dest, char * src){
    int slen = dest->len;
    int i = 0;
    char * new = (char*)malloc(strLen(src) + slen + 1);
    strcpy(new, dest->s);
    while (src[i]) {
        new[slen + i] = src[i];
        i++;
    }
    dest->len += i;
    new[dest->len] = 0;
    free(dest->s);
    dest->s = new;
}


String ** split(String * str, char c){
    int i;
    int j;
    String * new;
    int inquotes = 0;
    char * input = str->s;
    char tempStr[48];
    String ** dest;
    int numStrs = 1;
    for (i = 0; i < str->len; i++){
        if (input[i] == '"') inquotes = !inquotes;
        if (input[i] == c && !inquotes) {
            numStrs++;
        }
    }
    numStrs++;
    dest = (String**)malloc(sizeof(String*) * numStrs);

    i = 0;
    while (*input){
        j = 0;
        while (input[j] && (inquotes || input[j] != c) && j < 127){
            if (input[j] == '"') {
                inquotes = !inquotes;
                input++; // skip quote
                continue;
            }
            tempStr[j] = input[j];
            j++;
        }
        tempStr[j] = 0;
        input += j;
        if (*input == c) input++;
        new = newString(tempStr);
        dest[i] = new;
        i++;
    }
    dest[i] = 0;
    return dest;
}


int stringEqual(String * s1, String * s2){
    return strEqual(s1->s, s2->s);
}