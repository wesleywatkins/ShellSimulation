/*
  Project 1 for COP4610
  Header File
  ---------------------
  Brent Griffin
  Wesley Watkins
  --------------------
*/

#ifndef PROJ1_H
#define PROJ1_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef int bool;
#define true 1
#define false 0


//---Custom Function Templates---
void printPrompt();
void parseInput(char * line, int * aliasCount, char ** aliasList, 
	char ** cmdList, int * pidList, int * pidCount, int * commandCount);
char * resolveShortcut(char * filePath);
void execute(char * command, char ** cmdList, int * pidList, int * pidCount);  
void bgProc(char * userInput);

//---Execution Function Templates---
void ls(char * path);

//---Built-In Command Function Templates---
void exit_program(char ** cmdList, int * pidList, int * pidCount, int * commandCount);
void changeDirectory(char * path);
void echo(char * arg);
void alias(char * aliasDef, int * aliasCount, char ** aliasList);
void unalias(char * aliasDef, int * aliasCount, char ** aliasList);

//---Helper Function Templates---
char ** split(char * str, char * delim, int * count); 
bool checkBGProc(char ** cmdList, int * pidList, int * pidCount);
char * pathCat(char * str1, char * str2); 
bool exists(const char * fName); 
bool isFile(const char * fName);
bool isDirectory(const char * dName);
char * removeTrail(char * path); 
char * removeTrailSpace(char * str); 
char * removeLeadSpace(char * str); 
char * removeSpace(char * str);
bool isPipe(char * str);
void expandEnvr(char * str); 
void expandEnvrList(char ** arr); 

#endif
