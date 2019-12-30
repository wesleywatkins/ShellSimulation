/*
  Project 1 for COP4610
  Helpder Functions File
  ---------------------
  Brent Griffin
  Wesley Watkins
  --------------------
*/

#include "proj1.h"

//---Definitions for helper functions---

//Function for splitting a string into an array of strings
char** split(char * str, char * delim, int * count)
{
  //counter variable to keep track of array size
  *count = 0; 
  //temporary value to hold each segment of the string
  char * value; 
  //temporary array to hold segments 
  char ** arr = (char**)calloc(200,sizeof(char));

  // Copy str to not change the original
  char * temp = (char*)calloc(strlen(str)+1,sizeof(char));
  strcpy(temp,str);
	
  //discard newline from input 
  value = strtok(temp, "\n");

  //grab text, insert into array by segment
  value = strtok(value, delim);
  while(value != NULL || count == 0)
  {	
    arr[*count] = (char*)calloc((strlen(value)+1),sizeof(char));
    strncpy(arr[*count], value, strlen(value)+1);
    value= strtok(NULL, delim);
    *count = *count + 1;
  }
	
  //rallocate memory and store final result
  char ** result = (char**)calloc(*count+1,sizeof(char**));
  result = (char**)realloc(arr, (*count+1) * sizeof(*arr));
  //result[*count+1] = NULL;

  return result;
}

// Check running background processes
bool checkBGProc(char ** cmdList, int * pidList, int * pidCount) {
  bool result = false;
  if (*pidCount == 0) return true;
  int i, status;
  for (i = 0; i < *pidCount; i++) {
    pid_t status = waitpid(pidList[i], &status, WNOHANG);
    if (status == pidList[i]) {
      printf("\n[%d]+ [%s]\n", (i+1), cmdList[i]);
      // shift array over
      if (i < (*pidCount)-1) {
        int j;
        for (j = i; j < (*pidCount)-1; j++) {
          pidList[i] = pidList[i+1];
          free(cmdList[i]);
          cmdList[i] = (char*)calloc(strlen(cmdList[i+1])+1,sizeof(char));
          strcpy(cmdList[i],cmdList[i+1]);
        }
      }
      else
        free(cmdList[i]);
      (*pidCount)--;      
    }
  }
  return false;
}

//Function for concatenating two paths
char * pathCat(char * str1, char * str2)
{	
  //string to hold results
  char * str3 = (char*)calloc(strlen(str1)+strlen(str2)+2,sizeof(char));
  //copy first string into second string
  strncpy(str3, str1, strlen(str1) + 1); 
  //append backslash if needed
  if(str3[strlen(str3)] != '/' && str2[0] != '/')
    strncat(str3,"/", 1); 
  //append second string 
  strncat(str3, str2, strlen(str2) + 1); 
  return str3;
}

//Function to check if file or directory exists 
bool exists(const char * fName)
{ 
  FILE *file; 

  if((file = fopen(fName,"r")) == NULL)
    return false;

  return true; 
} 

//Function to check if file is a regular file
bool isFile(const char * fName)
{
  struct stat sb; 
  stat(fName, &sb);

  return (S_ISREG(sb.st_mode)); 
}

//Function to check if file is a directory
bool isDirectory(const char * dName)
{
  struct stat sb; 
  stat(dName, &sb);

  return (S_ISDIR(sb.st_mode)); 
}

//Function to remove trailing directory from passed in path
char * removeTrail(char * path)
{	
  char * result = ""; 
  int count = 0; 
  char * delim = "/"; 
  char ** pathArr = split(path, delim, &count);
  strncpy(pathArr[count-1], "", 1); 
  count = count - 1;
  int i;
  for(i=0; i<count; i++)
  {
    if (strncmp(pathArr[i], "", 1) != 0)
      result = pathCat(result, pathArr[i]);
  }
  return result; 
}

//Function to remove trailing whitespace from passed in string
char * removeTrailSpace(char * str)
{ 
  int ws = 0; 
  int i = 0; 
  char lastLetter;  
  char * result = strtok(str, "\n");
  
  //count whitespaces at end of string
  for(i = strlen(str)-1; i>=0; i--)
  {
    if(str[i] != ' ')
      break; 
    ws = ws + 1;
  }

  //append null character at first whitespace
  str[strlen(str)-ws] = '\0';
  result = str; 
  return result;
}

//Function to remove leading whitespace from passed in string
char * removeLeadSpace(char * str)
{ 
  int ws = 0; 
  int i = 0, j = 0; 
  char firstLetter;  
  char * result = strtok(str, "\n");
  
  //count whitespaces in beginning of string
  for(i = 0; i<strlen(str); i++)
  {
    if(str[i] != ' ')
      break; 
    ws = ws + 1;
  }

  //shift elements to the left
  for(i = 0; i<ws; i++)
  {
    for(j=0; j<strlen(str); j++)
      str[j] = str[j+1]; 
  }

  //append null character
  str[strlen(str)] = '\0';
  result = str; 
  return result;
}

//Function to remove whitespace from beginning and end of string
char * removeSpace(char * str)
{ 
  removeTrailSpace(str); 
  char * result = removeLeadSpace(str); 
  return result; 
}

//Function to check if string is a pipe
bool isPipe(char * str)
{ 
  bool foundPipe = false; 
  for(int i = 0; i<strlen(str); i++)
  {
    if(str[i] == '|')
      return true;     
  }
  return false; 
}

//Function to check command is background process
bool isBg(char * str)
{ 
  bool foundBg = false; 
  for(int i = 0; i<strlen(str); i++)
  {
    if(str[i] == '&')
      return true;     
  }
  return false; 
}

void expandEnvr(char * str) 
{ 
  int i = 0; 
  while(str[i] != '\0')
  {
    if(str[0]=='$')
    { 
      str++; 
      char * temp = getenv(str);
      str = (char*)calloc(strlen(temp)+1, sizeof(char));
      strncpy(str, temp, strlen(temp)+1);
    }
  i = i + 1; 
  }
}

//Function to find enviornment variables in an array and expand them
void expandEnvrList(char ** arr)
{ 
  int i = 0; 
  while(arr[i] != NULL)
  {
    if(arr[i][0]=='$')
    { 
      arr[i]++; 
      char * temp = getenv(arr[i]);
      arr[i] = (char*)calloc(strlen(temp)+1, sizeof(char));
      strncpy(arr[i], temp, strlen(temp)+1);
    }
    i = i + 1; 
  }
}