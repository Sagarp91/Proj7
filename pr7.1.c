/* CMPSC 311 Project 7 starter kit, version 1
 *   This differs slightly from the code in the project description.
 *
 * This file is provided as part of the project description and starter kit.
 * If you modify it, then put your name, email address, and the date, in this
 * comment, and include this file in the materials you turn in.
 *
 * Usage:
 *   c99 -v -o pr7 pr7.1.c                        [Sun compiler]
 *   gcc -std=c99 -Wall -Wextra -o pr7 pr7.1.c    [GNU compiler]
 *
 *   pr7
 *   pr7%      [type a command and then return]
 *   pr7%      [type control-D to indicate end-of-input and thus exit the shell]
 *
 * What is wrong or missing here?
 *   Only one-word commands are accepted.
 *   The shell does not recognize commands for itself.
 *   The shell retains no state information of its own.
 *   Command success or failure is not communicated properly.
 *   Failures by fork() and wait() cause the shell to fail.
 *   The error messages are not helpful.
 *   The maximum input line is too short, the prompt could be better, etc., etc.
 *   The signal generated from control-C terminates the shell.
 *
 * include files        for
 *   stdio.h            printf(), fprintf(), fgets(), feof(), stdin
 *   stdlib.h           exit(), EXIT_SUCCESS
 *   unistd.h           fork(), execlp()
 *   string.h           strlen()
 *   sys/types.h        pid_t
 *   sys/wait.h         waitpid()
 */

/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 128
   /* maximum input line length */

/*----------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
  char buffer[MAXLINE];                    /* command-line information */

  pid_t child_pid;                         /* child process information */
  int child_status;

  printf("%s - type control-D to exit\n", argv[0]);     /* simple instructions */

  printf("%s%% ", argv[0]);                /* prompt */

  while (fgets(buffer, MAXLINE, stdin) != NULL)    /* input */
    {
      buffer[strlen(buffer) - 1] = '\0';   /* remove input newline */

      if ((child_pid = fork()) == -1)      /* run the command in a child process */
        {
          fprintf(stderr, "fork failed\n"); break;      /* parent gives up */
        }
      else if (child_pid == 0)
        {
          execlp(buffer, buffer, NULL);    /* replace the child program */
          fprintf(stderr, "exec failed\n"); break;      /* child gives up */
        }

                                           /* parent waits for the child */
      if (waitpid(child_pid, &child_status, 0) == -1)
        {
          fprintf(stderr, "waitpid failed\n"); break;   /* parent gives up */
        }

      printf("%s%% ", argv[0]);            /* prompt */
    }

  if (feof(stdin))                         /* clean up the output after control-D */
    { printf("end of input\n"); }

  return EXIT_SUCCESS;
}

/*----------------------------------------------------------------------------*/

