/*
  Project 1 for COP4610
  Custom Functions File
  ---------------------
  Brent Griffin
  Wesley Watkins
  --------------------
*/

#include "proj1.h"



//---Definitions for custom functions---

/*
  Function to print the prompt USER@MACHINE: PWD >
*/

void printPrompt() 
{
  char * user = getenv("USER");
  char machine[50]; // for some reason, if this isn't an array, you get seg fault
  gethostname(machine, 50); 
  char * pwd = getenv("PWD");
  printf("%s@%s: %s > ", user, machine, pwd);
}

/*
  Function to parse the user's input and call the correct function according to
  the input (pass arguments as parameters)
*/

void parseInput(char * line, int * aliasCount, char ** aliasList, char ** cmdList, 
  int * pidList, int * pidCount, int * commandCount) 
{
  //initial variables
  char * command;
  char * execCommand; 
  char * lineCopy; 
  int lineCount, i;
  int firstElement = 1;
  bool foundAlias = false;
  
  //remove whitespace from end of line
  line = removeTrailSpace(line);

  // Remove Foreground Processing
  if (line[0] == '&') {
    line = strtok(line,"&");
    line = strtok(line," ");
  }

  //split input into an array
  char ** lineArr = split(line, " ", &lineCount);

  //get copy of line
  lineCopy = (char*)calloc(strlen(line)+1,sizeof(char));
  //save all input after the command, used for aliases
  strncpy(lineCopy, line, strlen(line)+1);  
  strtok(lineCopy, " "); 
  char * lineEnd = strtok(NULL, "");  

  //grab the command, first word in input
  command = (char*)calloc(strlen(lineArr[0])+1,sizeof(char));
  strcpy(command, lineArr[0]);
  
  //create empty string to store arguments
  char * args = (char*)calloc(0 ,sizeof(char));

  //first check for aliases
  if(aliasCount > 0)
  {
    for(i=0; i<*aliasCount; i++)
    {
      //allocate temporary space to store alias information
      char * aliasCopy = (char*)calloc(strlen(aliasList[i])+1,sizeof(char));
      strncpy(aliasCopy, aliasList[i], strlen(aliasList[i])+1);
      //get the name of the alias from alias array
      aliasCopy = strtok(aliasCopy, "=");

      if(strncmp(command, aliasCopy, strlen(command)) == 0)
      { 
        //get the command associated with the alias  
        aliasCopy = strtok(NULL, "'");

        //for external commands, store the whole string
        execCommand = (char*)calloc(strlen(aliasCopy)+1,sizeof(char));
        strncpy(execCommand, aliasCopy, strlen(aliasCopy)+1); 
        
        //parse the string for built-in commands (command and args)
        command = strtok(aliasCopy, " ");
        
        //store args for built-in commands
        char * tempArgs= strtok(NULL, "");
        
        //store args for built-in commands in array 
        lineArr[0] = (char*)calloc(strlen(tempArgs)+1,sizeof(char)); 
        strncpy(lineArr[0], tempArgs, strlen(tempArgs)+1); 
        
        //first element in array is now slot 0, default is slot 1
        firstElement = 0; 
        //set alias flag (used for external commands)
        foundAlias = true;
      }
    }
  }
  
  //convert lineArr array to an args string
  for(i = firstElement; i<lineCount; i++)
  {
    if(strncmp(lineArr[i], " ", 1) != 0)
    {
      char * tempArgs = (char*)realloc(args, strlen(args) + (strlen(lineArr[i]) * sizeof(char)));
      strncat(tempArgs, (lineArr[i]), (strlen(lineArr[i])));
      strncat(tempArgs, " ", 1);
      args = tempArgs; 
    }
  }

  //check for cd command 
  if(strncmp(command, "cd", 3) == 0)
    changeDirectory(args);

  //check for exit command
  else if(strncmp(command, "exit", 5) == 0)
    exit_program(cmdList,pidList,pidCount, &(*commandCount)); 

  //check for echo command
  else if(strncmp(command, "echo", 5) == 0)
    echo(args);  

  //check for alias command 
  else if(strncmp(command, "alias", 5) == 0)
    alias(args, &(*aliasCount), aliasList);

  //check for alias command 
  else if(strncmp(command, "unalias", 8) == 0)
    unalias(args, &(*aliasCount), aliasList);
  
  // Must be an execution command
  else 
  {
    if(foundAlias == false)
      execCommand = line; 
    else
    {  
      //check for input after alias 
      if(lineEnd != NULL)
      {
        char * tempExec = (char*)calloc((strlen(execCommand) + strlen(lineEnd)) + 2, sizeof(char)); 
        strncat(tempExec, execCommand, strlen(execCommand)+1); 
        strncat(tempExec, " ", 1); 
        strncat(tempExec, lineEnd, strlen(lineEnd)+1);
        execCommand = tempExec;   
      }
    }
    execute(execCommand,cmdList,pidList,&(*pidCount));
  }

  //increment number of commands counted
  *commandCount = *commandCount + 1; 
  //free up allocated memory
  for(i=1; i<lineCount; i++)
    free(lineArr[i]);

}


/*
  Function to execute commands
*/

void execute(char * command, char ** cmdList, int * pidList, int * pidCount)
{
  //Useful Variables
  bool output = false, input = false, pipeFlag = false, bgFlag = false, threePipes = false;
  int out = 0, in = 0, start = 0;
  int i;

  // Copy Command
  char * command_copy = (char*)calloc(strlen(command)+1,sizeof(char));
  strcpy(command_copy,command);

  //Get Command
  int count = 0;
  char ** tokens = split(command," ", &count);
  char * cmd = (char*)calloc(strlen(tokens[0])+1,sizeof(char));
  strcpy(cmd,tokens[0]);

  // Check For BG Processing
  if (strcmp(tokens[count-1],"&") == 0) {
    bgFlag = true;
    count--;
    command_copy[strlen(command_copy)-2] = 0;
  }

  //Create another array to check for piping
  int pCount = 0; 
  int fd[2];
  char ** pipeArr = split(command,"|", &pCount);
  pipeFlag = isPipe(command); 

  //remove spaces from pipe commands
  for(i=0; i<pCount; i++)
    removeSpace(pipeArr[i]);  

  //check number of commands
  if (pCount == 3)
    threePipes=true; 

  
  //invalid number of pipe commands
  else if (pCount<2 && pipeFlag==true)
  {
    printf("\nError: Invalid use of pipe command.\n");
    return; 
  }
  
  //Get "Absolute" Path
  char * path;
  char * bin = "/bin/";
  path = (char*)calloc(strlen(cmd) + strlen(bin) + 1, sizeof(char));
  strcpy(path, bin);
  strcat(path, cmd); 

  //Handle Arguments
  char * args[50];
  args[0] = (char*)calloc(strlen(path)+1,sizeof(char));
  strcpy(args[0],path);

  if(pipeFlag == false)
  {
    //Set Arguments and Check For Special Characters
    int arg_counter = 1;
    for (i = 1; i < count; i++) {
      if(strncmp(tokens[i], "<", 1) == 0) {
        input = true;
        in = open(tokens[i+1], O_RDONLY);
        i++;
      }
      else if(strncmp(tokens[i], ">", 1) == 0) {
        output = true;
        out = open(tokens[i+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        i++;
      }
      else {
        args[arg_counter] = (char*)calloc(strlen(tokens[i])+1,sizeof(char));
        strcpy(args[arg_counter],tokens[i]);
        arg_counter++;
      }
    }
    args[arg_counter] = NULL;

    //Calling ExecV
    int status;
    pid_t pid1 = fork();

    // Parent Process
    if (pid1 != 0) {
      if (bgFlag) {
        waitpid(pid1,&status,WNOHANG);
        cmdList[*pidCount] = (char*)calloc(strlen(command_copy)+1, sizeof(char));
        strcpy(cmdList[*pidCount],command_copy);
        pidList[*pidCount] = pid1;
        (*pidCount)++;
        printf("[%d] [%d]\n",*pidCount,pid1);
        if(checkBGProc(cmdList,pidList,pidCount))
          printPrompt();
      }
      else
        waitpid(pid1,&status,0);
    }

    // Child Process
    else {
      if (output) {
        dup2(out,1);
        close(out);
      }
      if (input) {
        dup2(in,0);
        close(in);
      }
      execv(args[0], args);
    }

  }

   //Piping
  else
  { 
    //counters for pipe arrays
    int pcCount1 = 0, pcCount2 = 0, pcCount3 = 0; 
    
    //arrays to hold split pipe commands
    char ** p1cmd;
    char ** p2cmd; 
    char ** p3cmd; 
    char * tempPath3; 

    //prepare arguments for pipe command 1 execution
    char * tempPath1 = (char*)calloc(strlen(bin) + strlen(pipeArr[0]) + 1, sizeof(char)); 
    strncat(tempPath1, bin, strlen(bin) + 1);
    strncat(tempPath1, pipeArr[0], strlen(pipeArr[0]) + 1);
    pipeArr[0] = tempPath1; 
    p1cmd = split(pipeArr[0], " ", &pcCount1); 

    //prepare arguments for pipe command 2 execution
    char * tempPath2 = (char*)calloc(strlen(bin) + strlen(pipeArr[1]) + 1, sizeof(char)); 
    strncat(tempPath2, bin, strlen(bin) + 1);
    strncat(tempPath2, pipeArr[1], strlen(pipeArr[1]) + 1);
    if(bgFlag)
    {
      strtok(tempPath2, "&");

    }
    pipeArr[1] = tempPath2; 
    p2cmd = split(pipeArr[1], " ", &pcCount2);
    
    //prepare arguments for (optional) pipe command 3 execution
    if(threePipes)
    {
      tempPath3 = (char*)calloc(strlen(bin) + strlen(pipeArr[2]) + 1, sizeof(char)); 
      strncat(tempPath3, bin, strlen(bin) + 1);
      strncat(tempPath3, pipeArr[2], strlen(pipeArr[2]) + 1);
      pipeArr[2] = tempPath3; 
      p3cmd = split(pipeArr[2], " ", &pcCount3);
    }


    //three commands for pipe
    if(threePipes)
    {
      int pd[2];
      int pd2[2];
      pid_t pid1 = fork();  
      int status; 
      pipe(pd);
      pipe(pd2); 
      
      if(pid1==0)
      { 
        //child-a 
        pid_t pid2 = fork();
        if (pid2 == 0)
        { 
          //child-b
          pid_t pid3 = fork();
          if(pid3==0)
          {   
            //child-c (third command)
            close(pd[1]);
            close(STDIN_FILENO);
            dup(pd2[0]);
            execv(*p3cmd, p3cmd);
          }

          else 
          {
            //parent-c (second command)
            close(pd[1]);
            close(STDIN_FILENO);
            dup(pd[0]);
            close(STDOUT_FILENO);
            dup(pd2[1]);
            execv(*p2cmd, p2cmd);
          }
        }

        else
        { 
          //parent-b (first command)
          close(pd[0]);
          close(STDOUT_FILENO);
          dup(pd[1]);
          execv(*p1cmd, p1cmd);
        }
      }
      //parent-a
      else
      {
        waitpid(pid1, &status, 0);  
        close(pd[0]);
        close(pd[1]);
        close(pd2[0]);
        close(pd2[1]);
      }        
    }

    //only two commands for pipe
    else
    {
      int pd[2];
      int status2; 
      pid_t pid1 = fork();
      pipe(pd);
      
      if(pid1==0)
      {   
        //child-a
        pid_t pid2 = fork();
        if (pid2==0)
        { 
          //child-b
          wait(&status2); 
          close(STDIN_FILENO);
          dup(pd[0]);
          close(pd[0]);
          close(pd[1]);
          execv(*p2cmd, p2cmd);
        }

        else
        {
          //parent-b
          close(STDOUT_FILENO);
          dup(pd[1]);
          close(pd[0]);
          close(pd[1]);
          execv(*p1cmd, p1cmd);
        }
      }

      else
      { 
        //parent-a
        if (bgFlag) 
        {
          waitpid(pid1,&status2,WNOHANG);
          cmdList[*pidCount] = (char*)calloc(strlen(command_copy)+1, sizeof(char));
          strcpy(cmdList[*pidCount],command_copy);
          pidList[*pidCount] = pid1;
          (*pidCount)++;
          printf("[%d] [%d]\n",*pidCount,pid1);
        }
        else
          waitpid(pid1, &status2, 0);
        return;
      }
      }
    }
}


/*
  Function to convert shortcuts to absolute file path names
*/

char * resolveShortcut(char * filePath)
{    
  int i, j;
  char * tempPath = removeTrailSpace(filePath); 
  strncpy(filePath, tempPath, strlen(tempPath)+1);

  //string to store expanded filepath
  char * result;
  //delimeter for split string function
  char * delim = "/"; 
  //counters for arrays
  int pCount = 0; 
  int fpCount = 0; 
  int elemCount = 0; 

  //gets paths for home and pwd
  char * pwd = getenv("PWD");
  char * home = getenv("HOME");

  //make copy of pwd so we dont change it
  char * pwdCopy = (char*)calloc((strlen(pwd)+1) ,sizeof(char));
  strcpy(pwdCopy, pwd);
  
  //split filepath of pwd
  char ** pArr = split(pwdCopy, delim, &pCount);
  char * parent = pArr[pCount-2];
  char * current = pArr[pCount-1];

  //split filepath of argument string into an array
  char ** fpArr = split(filePath, delim, &fpCount);
  //expand all enviornment variables in argument array
  expandEnvrList(fpArr); 
  for(int i = 0; i<fpCount; i++)
  {
    if(fpArr[i][0] == '/')
    {
      char ** tempElem = split(fpArr[i], delim, &elemCount); 
      //for(j=0; j)
    }
  }
  
  /* 
    these bools will specify whether the relative parent or current
    path is specifed. I.e. whether "../" or "./" is at the start of 
    the path 
  */ 
  bool goParent = false;
  bool goCurrent = false; 

  //check if parent shortcut is used on root directory
  if (strncmp (filePath, "/..", 3)==0)
  {
    printf("\nError: You cannot use parent-shortcut on root directory.\n");
    return 0;
  }

  //check for relative paths
  if(filePath[0] != '/')
  {  
    //check for parent directory shortcut (relative)
    if((strncmp ("..", fpArr[0], 2)) == 0)
      goParent = true; 

    //check for current directory shortcut (relative)
    else if((strncmp (".", fpArr[0], 1) == 0) && (strncmp ("..", fpArr[0], 2) != 0))
      goCurrent = true;

    //prepend pwd to fpArr[0]
    char * temp = (char*)calloc((strlen(fpArr[0])+1),sizeof(char));
    strcpy(temp, fpArr[0]);
    fpArr[0] = (char*)calloc((strlen(pwd)+strlen(fpArr[0])+1),sizeof(char));

    char * newPath; 
    //expand home 
    if(filePath[0] == '~')
      newPath = home; 
    //expand pwd
    else
      newPath = pathCat(pwd, temp); 

    strncat(fpArr[0], newPath, (strlen(newPath)+strlen(temp)+1));
  }

  //check for root shortcut at start of path
  else if(filePath[0] == '/' && strlen(filePath)==1)
    return "/"; 

  /* 
    remove the parent directory and the 
    '..' from the absolute path if '../' was 
    entered at the beginning of the path
  */ 
  if(goParent == true)
  { 
    fpArr[0] = removeTrail(fpArr[0]);
    fpArr[0] = removeTrail(fpArr[0]); 
  }

  /* 
    remove the last directory from the path if ./ 
    was entered at the beginning of the path
  */ 

  else if(goCurrent == true)
    fpArr[0] = removeTrail(fpArr[0]);

  result = fpArr[0];

  //for loop to expand shortcuts
  for(i=1; i<fpCount; i++)
  {   
    //expand parent--remove last directory from result string
    if(strncmp(fpArr[i], "..", 2) == 0)
      result = removeTrail(result);

    //expand current--nothing to modify
    if(strncmp(fpArr[i], ".", 1) == 0)
      continue; 
    
    //else append current directory to result string
    else
      result = pathCat(result, fpArr[i]);
 }

  //prepend backslash to result
  if(result[0] != '/')
    result = pathCat("", result);

  /*
    we copy the result to another string so that we can free
    the allocated memory in the split string array
  */ 
  char * temp = (char*)calloc((strlen(result)+1),sizeof(char));
  strncpy(temp, result, strlen(result)+1);
  result = temp; 

  //free allocated memory 
  for (i = 0; i<pCount; i++)
    free(pArr[i]); 
  for (i = 0; i<fpCount; i++)
    free(fpArr[i]);
  free(pwdCopy);

  //return final result string
  return result; 
}

//Function for input redirection
char *  redirectInput(char * userInput)
{ 
  char result[200];
  FILE * inFile = fopen (userInput, "r");
  fgets(result, 200, (FILE*)inFile);
  int count = ftell(inFile);
  char * str = (char*)calloc(count + 1,sizeof(char));
  strncpy(str, result, count);
  str[count + 1] = '\0'; 
  return str; 
} 