/*
  Project 1 for COP4610
  Main Function File
  ---------------------
  Brent Griffin
  Wesley Watkins
  --------------------
*/

#include "proj1.h"


//---Main Function---
int main() 
{
  char line[200];
  char * aliasList[10]; //list of strings to the aliases
  int aliasCount = 0; //integer to keep track of alias count
  char * cmdList[100];
  int pidList[100];
  int pidCount = 0;
  int commandCount = 0; 

  printf("\n\n");
  printPrompt();
  while (fgets(line,200,stdin)) 
  { 
    checkBGProc(cmdList,pidList,&pidCount);
    strtok(line, "\n"); //remove newline
    parseInput(line, &aliasCount, aliasList, cmdList, pidList, &pidCount, &commandCount);
    if(!isPipe(line))
      printPrompt();
  }

}
//end of main
