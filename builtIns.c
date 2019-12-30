/*
  Project 1 for COP4610
  Built-In Functions File
  ---------------------
  Brent Griffin
  Wesley Watkins
  --------------------
*/

#include "proj1.h"

//---Definitions for built-in functions---

//function for exiting
void exit_program(char ** cmdList, int * pidList, int * pidCount, int * commandCount)
{
  if (!checkBGProc(cmdList,pidList,pidCount))
    printf("Exiting...\n");
  while (!checkBGProc(cmdList,pidList,pidCount)) {}
  printf("Commands executed: %d\n", *commandCount); 
  exit(0);
}

//Function for changing CWD
void changeDirectory(char * path)
{ 
  char * abs_path = resolveShortcut(path);
  
  //check that file exists
  if(exists(abs_path)==false)
    printf("\nError: %s does not exist.\n", abs_path);

  else if(isDirectory(abs_path)==false)
    printf("\nError: %s is not a directory.\n", abs_path);
  
  if (chdir(abs_path) == 0) 
    setenv("PWD",abs_path,1);
}

//Function for echoing
void echo(char * arg)
{ 
  
  char * envv; 
  int argCount, i; 
  char ** argArr = split(arg, " ", &argCount);

  expandEnvrList(argArr);
  for(i = 0; i<argCount; i++)
  {
    printf("%s ", argArr[i]); 
    free(argArr[i]);
  }
  printf("\n");

}

//Function for adding alias
void alias(char * aliasDef, int * aliasCount, char ** aliasList)
{
  //allocate enough space for alias definition in the array slot
  aliasList[*aliasCount] = (char*)calloc((strlen(aliasDef)+1),sizeof(char));
  //copy alias definition into array slot
  strncpy(aliasList[*aliasCount], aliasDef, strlen(aliasDef+1)); 
  *aliasCount = *aliasCount + 1; 
  //expand enviornment variables in alias list
  expandEnvrList(aliasList); 
}

//Function for removing alias
void unalias(char * aliasDef, int * aliasCount, char ** aliasList)
{
  int i;
  for(i=0; i<*aliasCount; i++)
  {
    //allocate temporary space to store alias information
    char * aliasCopy = (char*)calloc(strlen(aliasList[i])+1,sizeof(char));
    strncpy(aliasCopy, aliasList[i], strlen(aliasList[i])+1);
    //get the name of the alias from alias array
    aliasCopy = strtok(aliasCopy, "=");

    if(strncmp(aliasCopy, aliasDef, strlen(aliasDef)) == 0)
    {
      free(aliasList[i]); 
      printf("\nAlias %s has been removed.\n", aliasDef);
    }
  }
}
