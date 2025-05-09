#ifndef COMMANDS_H
#define COMMANDS_H

void typeWriterCmd(String ** arguments, int numOptions);

void bounce(String ** arguments, int numOptions);

void hop(String ** arguments, int numOptions);

void pwdCmd(String ** arguments, int numOptions);

void changeColor(String ** arguments, short numOptions);

void lsCmd(String ** arguments, int numOptions);

void cdCmd(String ** arguments, int numOptions);

void mkdirCmd(String ** arguments, int numOptions);

void touchCmd(String ** arguments, int numOptions);

void echo(String ** arguments, int numOptions);

void rmCmd(String ** arguments, int numOptions);

void clear(String ** arguments, short numOptions);

void help(String ** arguments, short numOptions);

void halt();

void reset(String ** arguments, short numOptions);

void error(String * command);

#endif