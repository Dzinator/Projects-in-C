/*
	Yanis Hattab
	
	Basic custom command line for UNIX
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */


void parseCommand(char command[], char *args[]){
  //This method will parse a command in history and return it as an args array
  int i,
      start,
      next;

  for(i=0;i<41;i++){
    args[i] = NULL;
  }
  next = 0;
  start = -1;
    for (i = 0; i < 75; i++) { 
      switch (command[i]){

        case ' ':              
          if(start != -1){
             args[next] = &(command[start]);
             next++;
          }
          command[i] = '\0';
          start = -1;
          break;

        case '\n': 
          if (start != -1){
           args[next] = &command[start];     
           next++;
          }
          command[i] = '\0';
          args[next] = NULL;
          return;

        default :            
          if (start == -1) start = i;
      } 
    } 
    args[next] = NULL;

}



/**
 * setup() reads in the next command line, separating it into distinct
 * tokens using whitespace (space or tab) as delimiters. setup() sets
 * the args parameter as a null-terminated string.
 */
int setup(char inputBuffer[], char *args[], int *background)
{
  int length, /* # of characters in the command line */
    i,        /* loop index for accessing inputBuffer array */
    start,    /* index where beginning of next command parameter is */
    ct;       /* index of where to place the next parameter into args[] */
    
  int nbArgs = 0; //nb of arguments, if < 0 there was an error


  ct = 0;
    
  /* read what the user enters on the command line */
  length = read(STDIN_FILENO, inputBuffer, MAX_LINE);  

  start = -1;
  if (length == 0) {
    /* ctrl-d was entered, quit the shell normally */
    printf("\n");
    exit(EXIT_SUCCESS);
  } 
  if (length < 0) {
    /* somthing wrong; terminate with error code of -1 */
    perror("Reading the command");
    return -1; 
  }

  /* examine every character in the inputBuffer */
  for (i = 0; i < length; i++) { 
    switch (inputBuffer[i]){
    case ' ':
    case '\t':               /* argument separators */
      if(start != -1){
	       args[ct] = &inputBuffer[start];    /* set up pointer */
	       ct++;
         nbArgs++;
      }
      inputBuffer[i] = '\0'; /* add a null char; make a C string */
      start = -1;
      break;
    case '\n':                 /* should be the final char examined */
      if (start != -1){
	     args[ct] = &inputBuffer[start];     
	     ct++;
       nbArgs++;
      }
      inputBuffer[i] = '\0';
      args[ct] = NULL; /* no more arguments to this command */
      return nbArgs;
    default :             /* some other character */
      if (inputBuffer[i] == '&'){
      	*background  = 1;
      	inputBuffer[i] = '\0';
        start = -1;
      } else if (start == -1)
	start = i;
    } 
  }    
  args[ct] = NULL; /* just in case the input line was > MAX_LINE */
  return nbArgs;
} 



int main(void)
{
pid_t pid;
int *status = (int *)malloc(sizeof(int));

int commandNB = 0;
char commands[10][80]; //I story commands in history here
bool isError[10]; //this array will keep track of which of the commands in history are erroneous

char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
int background; /* equals 1 if a command is followed by '&' */
char *args[MAX_LINE/2 +1];
/* command line (of 80) has max of 40
arguments */

//I initialize the commands array that will store commands for history
int i;
  for(i=0;i<10;i++){
  //Initialize commands to NULL
  strcpy(commands[i], "\0");
  }

  for(i=0;i<10;i++){
    isError[i] = false;
  }


while (1){
/* Program terminates normally inside setup */
background = 0;
printf(" COMMAND->\n");

int nbArgs = setup(inputBuffer,args,&background); /* get next command */
if(nbArgs < 0) exit(EXIT_FAILURE); //exit on error

//I check if the inputBuffer is a built-in command
if(args[0] == NULL) continue; 

else if(strcmp(args[0], "history")==0) {
    for(i =9;i>=0;i--){
      printf("%s",commands[i]);
    }
    continue;
  }

else if(strcmp(args[0], "exit")==0) {
  printf("Good bye! :)\n");
  exit(EXIT_SUCCESS);
}

else if(strcmp(args[0], "cd")==0) {
  if(args[1] == NULL){
    printf("Please specify a directory\n");
    continue;
  }
  else{
    if(chdir(args[1]) != 0){
      printf("An error occured while attempting to change directory\n");
    }
    continue;
  }
}

else if(strcmp(args[0], "pwd")==0) {
  char *wd = getcwd(NULL, PATH_MAX);
  printf("Current directory is:  %s\n", wd);
  
  continue;
}

else if(strcmp(args[0], "jobs")==0) {
    execlp(args[0],args[1]);
    continue;
  }

else if(strcmp(args[0], "fg")==0) {
    execlp(args[0],args[1]);
    continue;
  }

else if(strcmp(args[0], "r")==0) {
  int num = 0; //this will hold the number fo the first command that matches
  char letter; 

  if(args[1] != NULL){ // a letter is specified
    letter = args[1][0];
    num = -1; //a flag, if it stays -1, no match was found in history
    for(i =0;i<10;i++){
       if(commands[i][5] == letter) num = i; //check the commands
    }
    if(num < 0){
     printf("No commands in history starting with '%c'\n", letter);
     continue;
    }
  }

  if(isError[num]){
    printf("The selected command is not valid\n");
    continue;
  }
  else{
    char *tempCommand;
    char tempCommand2[75];
    tempCommand = &(commands[num][5]);
    strcpy(tempCommand2, tempCommand);
    printf("%s ", tempCommand);
    parseCommand(tempCommand2,args);

  }
}




//A child process is spawned through fork, its pid is saved in the parent 
if((pid = fork()) < 0){
  perror("Error while creating child process");
  exit(EXIT_FAILURE);
}
else if(pid == 0){
  /*The child will try to start the program
  and if it fails to do so it will exit with an error
*/
  if(execvp(args[0],args)<0){
    perror("The command is invalid");
    exit(EXIT_FAILURE);
  }

}
else{
  //This shifts the commands in the array up & error status
  for(i =9;i>0;i--){
    strcpy(commands[i],commands[i-1]);
    isError[i] = isError[i-1];
  }
  isError[0] = false;

  //I give a number to the command and copy it
  commandNB++; 
  char *command = (char *) calloc(100, sizeof(char));
  sprintf(command, "%d)   %s ", commandNB, args[0]);
  for(i=1;i<40;i++){
  if(args[i] == NULL) break;
   if(strcmp(args[i],"\0") != 0){
      sprintf(command,"%s %s", command, args[i]);
   }
  } 
  strcat(command, "\n");
  strcpy(commands[0], command);

  //if not in the background, parent waits for child to finish
  if(background == 0){
    waitpid(-1,status,0);
    if(WEXITSTATUS(*status)  == 1) isError[0] = true;
    //The status of child is saved in status and if there was an error I flag the command
  }
  else{
    //here there was a &, I send a sleep signal to the process ...
    //... followed by a continue signal, thus 
    continue;

  } 
}
/* the steps are:
(1) fork a child process
using fork()
(2) the child process will invoke execvp()
(3) if background == 1, the parent will wait,
otherwise returns to the setup() function. */
}
}


