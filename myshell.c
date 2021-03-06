// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed:_____________________________________ Date:_____________

// 3460:426 Lab 1 - Basic C shell

/* Basic shell */

/*
 * This is a very minimal shell. It finds an executable in the
 * PATH, then loads it and executes it (using execv). Since
 * it uses "." (dot) as a separator, it cannot handle file
 * names like "minishell.h"
 *
 * The focus on this exercise is to use fork, PATH variables,
 * and execv. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS	64
#define MAX_ARG_LEN	16
#define MAX_LINE_LEN	80
#define WHITESPACE	" ,\t\n"


struct command_t {
   char *name;
   int argc;
   char *argv[MAX_ARGS + 1];
};

/* Function prototypes */
int parseCommand(char *, struct command_t *);
void printPrompt();
void readCommand(char *);

int main(int argc, char *argv[]) {
   int i;
   int exit;
   int pid;
   int lpid;
   int status;
   char cmdLine[MAX_LINE_LEN];


  
   struct command_t command;

   /* initiallize the entire char array to 0*/
   for(i = 0; i < MAX_ARGS + 1; ++i)
     argv[i] = 0;


   /* Shell initialization */

   exit = 1;
   while (exit == 1) {
      printPrompt();
      /* Read the command line and parse it */
      readCommand(cmdLine);

      parseCommand(cmdLine, &command);

      /* exit loop if user types exit */
      if(strcmp(command.name, "exit") == 0)
	break;
      
      /* Create a child process to execute the command
	 only if user hasnt exited the shell or
	 called the help function*/
      if(strcmp(command.name, "help") == 0){
	printf("******************************************\n");
	printf("Help Manual: Below are the list of commands supported\n");
	printf("%-20s ", "cls");
	printf("Clears the screen.\n");
	printf("%-20s ", "copy FILE1 FILE2");
	printf("Creates F2 and copies and bytes of F1 to F2 without deleting F1.\n");
	printf("%-20s ", "del FILE");
	printf("Deletes the named FILE.\n");
	printf("%-20s ", "dir");
	printf("Lists the contents of the current directory.\n");
	printf("%-20s ", "echo COMMENT");
	printf("Echoes the COMMENT back onto the screen.\n");
	printf("%-20s ", "exit");
	printf("Quits the shell.\n");
	printf("%-20s ", "make FILE");
	printf("Creates a text file named FILE by launching a text editor.\n");
	printf("%-20s ", "run PROGRAM");
	printf("Executes the named PROGRAM.\n");
	printf("%-20s ", "type FILE");
	printf("Displays the contents of the named FILE on screen.\n");
      }
      else
      	pid = fork();
      
      /*changed to an else if tree to add the catch all
	at the end for giving user help command*/
      if (pid == 0) {         
	
	/*setting the enviroment*/
	setenv("TERM", "linux", 0);
	
	if (strcmp(command.name, "cls") == 0)
	  command.name = "clear";
	else if (strcmp(command.name, "copy") == 0)
	  command.name = "cp";
	else if (strcmp(command.name, "del") == 0)
	  command.name = "rm";
	else if (strcmp(command.name, "dir") == 0) {
	  printf("\n");
	  /* need to create a child to display the
	     current directory or else it wont list
	     the contents of the directory*/
	  lpid = fork();
	  /*displays the current directory*/
	  if(lpid == 0)
	    execvp("pwd", command.argv);
	  wait(&lpid);
	  printf("\n");
	  /*setting the command to list the contents
	    in the long list form*/
	  command.argv[1]= "-l";
	  execvp("ls", command.argv);
	}
	else if (strcmp(command.name, "echo") == 0)
	  command.name = "echo"; 
	else if (strcmp(command.name, "make") == 0)
	  command.name = "nano";
	else if (strcmp(command.name, "run") == 0)
	  command.name = command.argv[1];
	else if (strcmp(command.name, "type") == 0)
	  command.name = "more";
	else{
	  printf("Command not recognized!\n");
	  printf("Please type 'help' for list of commands.\n");
	}
	
	
	/* Child executing command */
	command.argv[0] = command.name;
	

	execvp(command.name, command.argv);
      }
      /* Wait for the child to terminate */
      /* printf("%i ", pid);*/
      wait(&pid);		
      
   }

   /* Shell termination */
   printf("\n\n shell: Terminating successfully\n");
   return 0;
}

/* End basic shell */

/* Parse Command function */

/* Determine command name and construct the parameter list.
 * This function will build argv[] and set the argc value.
 * argc is the number of "tokens" or words on the command line
 * argv[] is an array of strings (pointers to char *). The last
 * element in argv[] must be NULL. As we scan the command line
 * from the left, the first token goes in argv[0], the second in
 * argv[1], and so on. Each time we add a token to argv[],
 * we increment argc.
 */
int parseCommand(char *cLine, struct command_t *cmd) {
   int argc;
   char **clPtr;
   /* Initialization */
   clPtr = &cLine;	/* cLine is the command line */
   
   argc = 0;
   cmd->argv[argc] = (char *) malloc(MAX_ARG_LEN);
   /* Fill argv[] */
   while ((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {
      cmd->argv[++argc] = (char *) malloc(MAX_ARG_LEN);
   }

   /* Set the command name and argc */
   cmd->argc = argc-1;
   cmd->name = (char *) malloc(sizeof(cmd->argv[0]));
   strcpy(cmd->name, cmd->argv[0]);
   return 1;
}

/* End parseCommand function */

/* Print prompt and read command functions - pp. 79-80 */

void printPrompt() {
   /* Build the prompt string to have the machine name,
    * current directory, or other desired information
    */
   char promptString[] = "linux (dcc53)|>";
   printf("%s ", promptString);
}

void readCommand(char *buffer) {
   /* This code uses any set of I/O functions, such as those in
    * the stdio library to read the entire command line into
    * the buffer. This implementation is greatly simplified,
    * but it does the job.
    */
   gets(buffer);
}

/* End printPrompt and readCommand */
