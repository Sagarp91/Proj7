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
#include "builtin.h"
                                 /* evaluate a command line */
int eval_line(char *cmdline, int e_flags);
                                 /* build the _argv array */
int parse(char *buf, char *_argv[]);
int builtin(char *_argv[]);      /* if builtin command, run it */

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
    char cmdline[MAXLINE];       /* command line */

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
                                 // Make sure to call free on startfile now
                startfile = Strdup(optarg);
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
    else // open the default init file, if it exists
    {
        f_p = fopen("pr7.init", "r");
        if(f_p == NULL)
        {
            fprintf(stderr, "%s: Cannot open file %s: %s\n", prog, filename, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // STARTFILE LOOP:
    while(fgets(cmdline, MAXLINE, f_p) != NULL)
    {
        /* issue prompt and read command line */
        if (feof(stdin))     /* end of file */
            { break; }

        if((ret = eval_line(cmdline, e_flag)) == EXIT_FAILURE)
            fprintf(stderr, "%s: failure in eval_line, on line %s. %s\n", prog, cmdline, strerror(errno));
    }

    fclose(f_p);

    // Now read from either a file or stdin

    // If a file is given, open it here
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

    // Else, no file was given, so just read from stdin.
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
        if (feof(stdin))         /* end of file */
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
    char *_argv[MAXARGS];        /* _argv for execve(), denoted by underscore to distinguish from main argv array*/
    char buf[MAXLINE];           /* holds modified command line */
    int background;              /* should the job run in background or foreground? */
    pid_t pid;                   /* process id */
    int ret = EXIT_SUCCESS;

    strcpy(buf, cmdline);        /* buf[] will be modified by parse() */
                                 /* build the argv array */
    background = parse(buf, _argv);

    if(e_flag > 0)
        Echo(_argv);

    if (_argv[0] == NULL)        /* ignore empty lines */
        { return ret; }

    if (builtin(_argv) == 1) /* the work is done */
        { return ret; }

    if(background)
    {
        block_signal(SIGINT, 1);
    }

                            /* child runs user job */
    if ((pid = fork()) == 0)
    {
        if (execvp(_argv[0], _argv/*, environ*/) == -1)
        {
            printf("%s: execve failed: %s\n", _argv[0], strerror(errno));
            _exit(EXIT_FAILURE);
        }
    }

    if (background)              /* parent waits for foreground job to terminate */
    {
        printf("background process %d: %s", (int) pid, cmdline);
        block_signal(SIGINT, 0);
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
    char *delim;                 /* points to first whitespace delimiter */
    int argc = 0;                /* number of args */
    int bg;                      /* background job? */

    char whsp[] = " \t\n\v\f\r"; /* whitespace characters */

    /* Note - the trailing '\n' in buf is whitespace, and we need it as a delimiter. */

    while (1)                    /* build the _argv list */
    {
        buf += strspn(buf, whsp);/* skip leading whitespace */
                                 /* next whitespace char or NULL */
        delim = strpbrk(buf, whsp);
        if (delim == NULL)       /* end of line */
            { break; }
            _argv[argc++] = buf; /* start _argv[i] */
        *delim = '\0';           /* terminate _argv[i] */
        buf = delim + 1;         /* start _argv[i+1]? */
    }
    _argv[argc] = NULL;

    if (argc == 0)               /* blank line */
        { return 0; }

    /* should the job run in the background? */
    if ((bg = (strcmp(_argv[argc-1], "&") == 0)))
        { _argv[--argc] = NULL; }

    return bg;
}

