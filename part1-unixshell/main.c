/**
* File: main.c 
* Description: Read one line at a time and execute commands
* Date: 6/5/2026
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "token.h"
#include "command.h"
#define COMMAND_LINE_SIZE 100 // Global variable (BAD) for command line size 

void pwd();
void cd(char * path);
void walk(char * path);
void change_prompt(char* newPrompt);
char prompt[256] = "% ";

int main(int argc, char *argv[]){
  // Initialize Variables
  char inputLine[COMMAND_LINE_SIZE];
  char *token[MAX_NUM_TOKENS];
  Command command[MAX_NUM_COMMANDS];

  // Ignore Ctrl+C, Ctrl+Z and Ctrl+\

  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGINT);
  sigaddset(&sigs, SIGQUIT);
  sigaddset(&sigs, SIGTSTP);
  sigprocmask(SIG_SETMASK, &sigs, NULL);

  while(1){
    printf("%s", prompt);

    // Breaks out the loop if fgets fails. 
    if(fgets(inputLine, COMMAND_LINE_SIZE, stdin) == NULL) 
      break;

    inputLine[strcspn(inputLine, "\n")] = '\0'; // Pattern for removing saved newline

    int tokenSize = tokenize(inputLine, token);
    
    // Checks for error 
    if(tokenSize == -1){
      printf("Too many tokens\n");
      continue; 
    }
    
    // Initalize command array 
    for(int n = 0; n < MAX_NUM_COMMANDS; n++){
      initializeCommandStructure(command);
    }

    int commandSize = separateCommands(token, command);

    // Print and execute the commands 
    for(int n = 0; n < commandSize; n++){
      printf("Command %d: ", n+1);

      for(int i = command[n].first; i <= command[n].last; i++){
        printf("%s ", token[i]);
      }
      printf("\n");
      printf("Separator: %s\n", command[n].sep);

      if (strcasecmp(command[n].argv[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
      } else if (strcmp(command[n].argv[0], "pwd") == 0) {
        pwd();
      } else if (strcmp(command[n].argv[0], "cd") == 0) {
        cd(command[n].argv[1]);
      } else if (strcmp(command[n].argv[0], "walk") == 0) {
        walk(command[n].argv[1]);
      } else if (strcmp(command[n].argv[0], "prompt") == 0) {
        change_prompt(command[n].argv[1]);
      } else {
        int pid = fork();
        if (/*this is the parent (the shell)*/ pid > 0) {
          //waitpid(pid);
          wait((int*)0);
        }
        if (/*this is the child (the command)*/ pid == 0) {
          execvp(command[n].argv[0], command[n].argv);
          perror("execv failed");
        }
        if (/*fork failed*/ pid < 0) {
          perror("fork");
        }
      }
    }
  }
  return 0;
}

//Prompts with spaces are not processed properly
void change_prompt(char* newPrompt) {
    strcpy(prompt, newPrompt);
    return;
}

void pwd() {
    char cwd[4096];
    getcwd(cwd, sizeof(cwd));
    if (cwd == NULL) {
        perror("getcwd failed");
        return;
    }
    printf("%s\n", cwd);
}

void cd(char * path) {
    if (path == NULL) {
        return;
    } else if (chdir(path) != 0) {
        perror("cd");
    }
    return;
}

void walk(char * path) {
    if (path == NULL) {
        if (chdir(getenv("HOME")) != 0) {
            perror("cd");
        }
        return;
    } else if (chdir(path) != 0) {
        perror("cd");
    }
    return;
}
/*
void history() {
    
}
*/
