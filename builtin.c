/*
 * builtin.c - Builtin functions implementation file
 * Erich Stoekl, CMPSC 311, SP12
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>


#include "cmpsc311.h"
#include "builtin.h"
//#include "pr7_table.h"

/* if first arg is a builtin command, run it and return true
 *
 * Compare to builtin_command() in CS:APP Fig. 8.23.
 */

extern char **environ;

int builtin(char *_argv[], table_t *pt)
{
                                 /* exit command */
    if (strcmp(_argv[0], "exit") == 0)
    {
        int children = number_of_children(pt);
        if(children > 0)
        {
            printf("There are %d background job(s) running.\n", children);
            return 1;
        }
        else
        {
            exit(0);
        }
    }

                                 /* ignore singleton & */
    if (strcmp(_argv[0], "&") == 0)
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
                // = (char *)malloc( (size_t)PATH_MAX );
                char resolved_path[PATH_MAX];
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
            // Check to see if this needs to be Strdup'd!
            if((env = getenv(_argv[1])) == NULL)
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
                                 // Check to see if this needs to be Strdup'd!
            if(setenv(_argv[1], _argv[2], 1) == -1)
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
                                 // Check to see if this needs to be Strdup'd!
            if(unsetenv(_argv[1]) == -1)
                fprintf(stderr, "%s: unsetenv failed: %s.\n", prog, strerror(errno));
        }
        else
            printf("%s: No argument given for unsenv", prog);
        return 1;
    }

    if(strcmp(_argv[0], "pjobs") == 0)
    {
        print_process_table(pt, __func__);
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

    return 0;                    /* not a builtin command */
}

int Echo(char *_argv[])
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
