/* CMPSC 311 Project 7 starter kit, version 2
 *
 * This file is provided as part of the project description and starter kit.
 * If you modify it, then put your name, email address, and the date, in this
 * comment, and include this file in the materials you turn in.
 *
 * Usage:
 *   c99 -v -o pr7 pr7.2.c                        [Sun compiler]
 *   gcc -std=c99 -Wall -Wextra -o pr7 pr7.2.c    [GNU compiler]
 *
 *   pr7
 *   pr7%      [type a command and then return]
 *   pr7% exit
 *
 * This version is derived from the shellex.c program in CS:APP Sec. 8.4.
 */

/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "cmpsc311.h"
#include "pr7_signal.h"

#define MAXLINE 128
#define MAXARGS 128

int eval_line(char *cmdline, int e_flags);                   /* evaluate a command line */
int parse(char *buf, char *_argv[]);             /* build the _argv array */
int builtin(char *_argv[]);                      /* if builtin command, run it */
static int Echo(char *_argv[]);

extern char **environ;

static void usage(int status)
{
    if (status == EXIT_SUCCESS)
    {
        printf("Usage:  pr7 [-h] [-v] [-i] [-e] [-s f] [file]\n");
        printf("-h     help\n");
        printf("-v     verbose mode\n");
        printf("-i     interactive mode\n");
        printf("-e     echo commands before execution\n");
        printf("-s f   use startup file f, default pr7.init\n");
    }
    else
    {
        fprintf(stderr, ": Try '-h' for usage information.\n");
    }
    exit(status);
}

// Need to install with install_signal_handler function
static void SIGINT_handler(int sig)
{
    if(sig == SIGINT)
    {
        printf("\n");
    }
    else
        printf("SERIOUS ERROR: SIGING_handler received signal %d\n", sig);
}


/*----------------------------------------------------------------------------*/

/* Compare to main() in CS:APP Fig. 8.22 */

int main(int argc, char *argv[])
{
    prog = argv[0];
  int ret = EXIT_SUCCESS;
  char cmdline[MAXLINE];                /* command line */

  // File pointer:
  extern FILE *f_p;
  extern char *filename;
  extern char *startfile;

  extern char *prompt;

  // Getopt section:
  extern int optind;
  extern char *optarg;

  int v_flag = 0;
  int i_flag = 0;
  int e_flag = 0;
  int s_flag = 0;
  int ch;

    optind = 1;
    while((ch = (getopt(argc, argv, ":hvies:"))) != -1)
    {
        switch(ch)
        {
            case 'h':
                usage(ret);
                break;
            case 'v':
                v_flag++;
                break;
            case 'i':
                i_flag++;
                break;
            case 'e':
                e_flag++;
                break;
            case 's':
                s_flag++;
                startfile = Strdup(optarg); // Make sure to call free on startfile now
                break;
            default:
                usage(EXIT_FAILURE);
                break;
        }
    }

    install_signal_handler(SIGINT, SIGINT_handler);

    if(s_flag > 0)
    {
        f_p = fopen(startfile, "r");
        if(f_p == NULL)
        {
            fprintf(stderr, "%s: Cannot open file %s: %s\n", prog, filename, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // STARTFILE LOOP:
    if(s_flag > 0)
    {
        while(fgets(cmdline, MAXLINE, f_p) != NULL)
        {
          /* issue prompt and read command line */
          if (feof(stdin))                  /* end of file */
            { break; }

          if((ret = eval_line(cmdline, e_flag)) == EXIT_FAILURE)
              fprintf(stderr, "%s: failure in eval_line, on line %s. %s\n", prog, cmdline, strerror(errno));
        }
    }

    if(argv[optind] != NULL)
    {
        // there is a filename given, so read from that only.
        f_p = fopen(argv[optind], "r");
        if(f_p == NULL)
        {
            fprintf(stderr, "%s: Cannot open file %s: %s\n", prog, filename, strerror(errno));
            exit(EXIT_FAILURE);
        }
        filename = argv[optind]; // ATTN: watch for leaks! might need to strdup.
    }

    else
    {
        f_p = stdin;
        filename = "[stdin]";
    }

    // MAIN PROGRAM LOOP:
    prompt = argv[0];
    if(f_p == stdin && i_flag > 0)
        printf("%s%% ", prompt);

    while(fgets(cmdline, MAXLINE, f_p) != NULL)
    {
        /* issue prompt and read command line */
        if (feof(stdin))                  /* end of file */
            { break; }

        if((ret = eval_line(cmdline, e_flag)) == EXIT_FAILURE)
          fprintf(stderr, "%s: failure in eval_line, on line %s. %s\n", prog, cmdline, strerror(errno));

        if(f_p == stdin && i_flag > 0)
            printf("%s%% ", prompt);
    }

      free(startfile);

  return ret;
}

/*----------------------------------------------------------------------------*/

/* evaluate a command line
 *
 * Compare to eval() in CS:APP Fig. 8.23.
 */

int eval_line(char *cmdline, int e_flag)
{
  char *_argv[MAXARGS];  /* _argv for execve(), denoted by underscore to distinguish from main argv array*/
  char buf[MAXLINE];    /* holds modified command line */
  int background;       /* should the job run in background or foreground? */
  pid_t pid;            /* process id */
  int ret = EXIT_SUCCESS;

  strcpy(buf, cmdline);                 /* buf[] will be modified by parse() */
  background = parse(buf, _argv);        /* build the argv array */

  if(e_flag > 0)
      Echo(_argv);

  if (_argv[0] == NULL)          /* ignore empty lines */
    { return ret; }

  if (builtin(_argv) == 1)       /* the work is done */
    { return ret; }

  if ((pid = fork()) == 0)      /* child runs user job */
    {
      if (execvp(_argv[0], _argv/*, environ*/) == -1)
        {
          printf("%s: execve failed: %s\n", _argv[0], strerror(errno));
          _exit(EXIT_FAILURE);
        }
    }

  if (background)               /* parent waits for foreground job to terminate */
    {
      printf("background process %d: %s", (int) pid, cmdline);
    }
  else
    {
      if (waitpid(pid, &ret, 0) == -1)
        {
          printf("%s: waitpid failed: %s\n", _argv[0], strerror(errno));
          exit(EXIT_FAILURE);
        }
    }

  return ret;
}

/*----------------------------------------------------------------------------*/

/* parse the command line and build the _argv array
 *
 * Compare to parseline() in CS:APP Fig. 8.24.
 */

int parse(char *buf, char *_argv[])
{
  char *delim;          /* points to first whitespace delimiter */
  int argc = 0;         /* number of args */
  int bg;               /* background job? */

  char whsp[] = " \t\n\v\f\r";          /* whitespace characters */

  /* Note - the trailing '\n' in buf is whitespace, and we need it as a delimiter. */

  while (1)                             /* build the _argv list */
    {
      buf += strspn(buf, whsp);         /* skip leading whitespace */
      delim = strpbrk(buf, whsp);       /* next whitespace char or NULL */
      if (delim == NULL)                /* end of line */
        { break; }
      _argv[argc++] = buf;               /* start _argv[i] */
      *delim = '\0';                    /* terminate _argv[i] */
      buf = delim + 1;                  /* start _argv[i+1]? */
    }
  _argv[argc] = NULL;

  if (argc == 0)                        /* blank line */
    { return 0; }

  /* should the job run in the background? */
  if ((bg = (strcmp(_argv[argc-1], "&") == 0)))
    { _argv[--argc] = NULL; }

  return bg;
}

/*----------------------------------------------------------------------------*/

/* if first arg is a builtin command, run it and return true
 *
 * Compare to builtin_command() in CS:APP Fig. 8.23.
 */

int builtin(char *_argv[])
{
  if (strcmp(_argv[0], "exit") == 0)     /* exit command */
    { exit(0); }

  if (strcmp(_argv[0], "&") == 0)        /* ignore singleton & */
    { return 1; }

  if(strcmp(_argv[0], "echo") == 0)
  {
      Echo(_argv + 1);
      return 1;
  }

  if(strcmp(_argv[0], "dir") == 0)
  {
      char *pwd_buf = (char *)malloc( (size_t)MAXLINE ); 
      if(getcwd(pwd_buf, MAXLINE) == NULL)
      {
          fprintf(stderr, "failed on getwd, line %d: %s\n", __LINE__, strerror(errno));
      }
      else
      {
          printf("%s\n", pwd_buf);
      }
      free(pwd_buf);
      return 1;
  }

  if(strcmp(_argv[0], "cdir") == 0)
  {
      if(f_p != stdin)
      {
          fprintf(stderr, "%s: cannot cdir from a script.\n", prog);
      }
      else
      {
          if(_argv[1] == NULL)
          {
              // No pathname was given, cd to HOME
              char *home_path;
              if((home_path = getenv("HOME")) == NULL)
                  fprintf(stderr, "%s: No environ name HOME.\n", prog);
              
              if(chdir(home_path) == -1)
              {
                  fprintf(stderr, "%s: chdir error: %s.\n", prog, strerror(errno));
              }
              else
              {
                  // We can safely change the environment variable PWD:
                  setenv("PWD", home_path, 1);
              }
              
          }

          else
          {
              // Translate path given into realpath:
              // Maybe ask heller about whether to put resolved_path on stack or heap
              char *path = Strdup(_argv[1]);
              char resolved_path[PATH_MAX];// = (char *)malloc( (size_t)PATH_MAX );
              realpath(path, resolved_path);

              if(chdir(resolved_path) == -1)
              {
                  fprintf(stderr, "%s: chdir error: %s.\n", prog, strerror(errno));
              }
              else
              {
                  // We can safely change the environment variable PWD:
                  setenv("PWD", resolved_path, 1);
              }
              
              free(path);
          }
      }
      return 1;
  }

  if(strcmp(_argv[0], "penv") == 0)
  {
      char *env;
      if(_argv[1] != NULL)
      {
          if((env = getenv(_argv[1])) == NULL) // Check to see if this needs to be Strdup'd!
              fprintf(stderr, "%s: No environ variable of that name exists.\n", prog);
          printf("%s=%s\n", _argv[1], env);
      }
      else
      {
          // Print all environment variables:
          int i;
          for(i = 0; environ[i] != '\0'; i++)
              printf("%s\n", environ[i]);

      }
      return 1;
  }

  if(strcmp(_argv[0], "senv") == 0)
  {
      if(_argv[1] != NULL && _argv[2] != NULL)
      {
          if(setenv(_argv[1], _argv[2], 1) == -1) // Check to see if this needs to be Strdup'd!
              fprintf(stderr, "%s: setenv failed: %s.\n", prog, strerror(errno));
      }
      else
          printf("%s: Insufficient arguments given for setenv", prog);
      return 1;
  }

  if(strcmp(_argv[0], "unsenv") == 0)
  {
      if(_argv[1] != NULL)
      {
          if(unsetenv(_argv[1]) == -1) // Check to see if this needs to be Strdup'd!
              fprintf(stderr, "%s: unsetenv failed: %s.\n", prog, strerror(errno));
      }
      else
          printf("%s: No argument given for unsenv", prog);
      return 1;
  }

  if(strcmp(_argv[0], "help") == 0)
  {
      printf("\tSpecial commands:\n");
      printf("\texit - kills shell\n");
      printf("\techo - prints remaining arguments\n");
      printf("\tdir - prints present working directory\n");
      printf("\tcdir - changes directory to path specified\n");
      printf("\tpenv - prints given environment variable,\n\tor all environment variables if none given\n");
      printf("\tsenv - set given environment variable name and variable\n");
      printf("\tunsenv - unset given environment variable name from environment variable list\n");

      return 1;
  }

  return 0;                             /* not a builtin command */
}

/*----------------------------------------------------------------------------*/

static int Echo(char *_argv[])
{
    int i;
    // Make sure this will never segfault
    for(i = 0; _argv[i] != NULL; i++)
    {
        if(_argv[i] == NULL)
            return -1;

        printf("%s ", _argv[i]);
    }
    printf("\n");
    return 0;
}

