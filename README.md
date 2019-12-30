# Project 1--COP4610
The purpose of this project is to familiarize you with the mechanics of process control through the implementation of a shell user interface.

# Contents
The contents of the tar archive and a brief description of each file. 
- **main.c**: Contains the main method for using the program.  
- **proj1.c**: Contains definitions for the core custom functions of the program. Includes functions such as parseInput(), shorcutResolution(), and execute(). 
- **builtIns.c**: Contains function definitions for all of the built-in commands.
- **helpers.c**: Contains function definitions for all of the helper functions. 
- **proj1.h**: The header file. Contains function prototypes for every function in the program (not just the functions defined in proj1.c).
- **README.md**: A formatted text file which contains a brief overview of the project implementation. 
- **makefile**: Utility file that compiles all of the source code. 

# How to compile
To compile the program, simply type ```make``` at the command line. This will create an executable named **p1**. Execute the program by typing ```./p1```. 

# Known Bugs
1. When redirecting, input/output, there must be a space between the I/O symbol and the commands. We ran out of time, and were not able to successfully seperate the redirection symbols from the commands. 
2. Enviornment variables do not get expanded correctly when passing them as arguments to the "cd" command. 
