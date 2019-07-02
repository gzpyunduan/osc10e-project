#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

char command[100];
char history[100];
char argv[50][50]; /* command line arguments */
int count = 0; /*number of command line arguments*/
enum specify{NORMAL, OUT_REDIRECT, IN_REDIRECT, PIPE};

int analysis_command();
int do_command();

int main(void)
{
  command[0] = 0;
  history[0] = 0;
  char sign[3];
  while (1) {
    printf("osh>");
    fgets(command, 100, stdin);
    sign[0] = command[0], sign[1] = command[1], sign[2] = '\0';
    if (strcmp(sign, "!!") == 0)
      if (history)
        strcpy(command, history);
      else
        printf("No commands in history.");
    //printf("%s", command);
    if(!analysis_command())
      continue;
    do_command();
    strcpy(history, command);


    /**
    * After reading user input, the steps are:
    * (1) fork a child process using fork()
    * (2) the child process will invoke execvp()
    * (3) parent will invoke wait() unless command included &
    */
  }
  return 0;
}

int analysis_command(){
  char *s = command;
  int i = 0, j = 0, state = 0;
  strcat(command, " ");

  while(*s){
    if (state == 0){
      if (!isspace(*s))
        state = 1;
      else
        s++;
    }
    else {
      if(isspace(*s)){
		    argv[i][j] = '\0';
		    i++;
		    j = 0;
		    state = 0;
	    }
      else{
		    argv[i][j] = *s;
		    j++;
	    }
	    s++;
    }
  }

  count = i;
  if (count == 0)
    return 0;

  if (strcmp(argv[0], "logout") == 0 || strcmp(argv[0], "exit") == 0)
    exit(0);

  return 1;
}

int do_command(){
  int i, j;
  char *file;
  char *arg[50];
  char *arg2[50];
  int f = 0, back_run = 0;
  int fd, pid, fd2, pid2;
  enum specify type = NORMAL;

  for (i = 0; i < count; i++)
    arg[i] = argv[i];
  arg[i] = NULL;

  if(strcmp(arg[count - 1], "<") == 0 || strcmp(arg[count-1],">") == 0 || strcmp(arg[count - 1], "|") == 0){
	  printf("error:command error\n");
	  return 0;
  }

  for(i = 0; i < count; i++){
	  if(strcmp(arg[i], "<") == 0){
      f++;
	    file = arg[i + 1];
	    arg[i] = NULL;
	    type = IN_REDIRECT;
	  }
    else if(strcmp(arg[i], ">") == 0){
	    f++;
	    file = arg[i + 1];
	    arg[i] = NULL;
	    type = OUT_REDIRECT;
	  }
    else if(strcmp(arg[i], "|") == 0){
	    f++;
	    type = PIPE;
	    arg[i] = NULL;
	    for(j = i + 1; j < count; j++)
		    arg2[j - i - 1] = arg[j];
	    arg2[j - i - 1] = NULL;
	  }
  }

  if(strcmp(arg[count - 1], "&") == 0){
	  back_run = 1;
	  arg[count - 1] = NULL;
  }

  if(f > 1){
	  printf("error:don't identify the command");
	  return 0;
  }

  pid = fork();
  if (pid < 0){
    perror("fork error");
    exit(0);
  }
  else if (pid == 0){
    switch(type){
      case NORMAL:
        execvp(arg[0], arg);
        break;
      case IN_REDIRECT:
        fd = open(file, O_RDONLY);
        dup2(fd, STDIN_FILENO);
        execvp(arg[0], arg);
        break;
      case OUT_REDIRECT:
        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        dup2(fd, STDOUT_FILENO);
        execvp(arg[0], arg);
        break;
      case PIPE:
        pid2 = fork();
        if (pid2 == 0){
          fd2 = open("tempfile", O_WRONLY | O_CREAT | O_TRUNC, 0600);
          dup2(fd2, STDOUT_FILENO);
          execvp(arg[0], arg);
        }
        else {
          waitpid(pid2, NULL, 0);
          fd = open("tempfile", O_RDONLY);
          dup2(fd, STDIN_FILENO);
          execvp(arg[0], arg);
        }
        break;
    }
  }
  else {
    if(!back_run)
      waitpid(pid, NULL, 0);
  }

  return 1;
}
