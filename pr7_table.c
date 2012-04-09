/*----------------------------------------------------------------------------*/

/* This is a rather primitive implementation.  It should be improved
 * by changing from a fixed-size array to a dynamically-allocated
 * linked list.
 *
 * The only trick here is the C99 pre-defined identifier __func__ which
 * evaluates to a character string giving the name of the function
 * in which it appears.  (See C:ARM Sec. 2.6.1)
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "pr7_table.h"

/* fixed-size process table, give the size as a symbolic constant */
#define MAX_CHILDREN 8

/* an entry in the process table */
/* The process table, maintained by the parent process only.
 * The table is static, to restrict access to the implementation.
 */
//static pr7_process_info process_table[MAX_CHILDREN];

/*
 * just for reference:
typedef struct pr7_process
{
    pid_t pid;
    int   state;
    int   exit_status;

    struct pr7_process *next;
} child_t;

typedef struct process_table
{
    int children;
    child_t *p_tab;
} table_t;
*/

#define STATE_NONE       0
#define STATE_RUNNING    1
#define STATE_TERMINATED 2

static char *state[] = { "none", "running", "terminated" };

int pr7_debug;

/*----------------------------------------------------------------------------*/

int number_of_children(table_t *pt)
{
    child_t *iter = pt->p_tab;
}

void print_process_table(table_t *pt, const char * const caller)
{
    // Create an iterator object:
    child_t *iter = pt->p_tab;

    printf("  in pid %d, process table, called from %s\n", getpid(), caller);
    printf("       pid         state        status\n");

    if (iter == NULL)
        printf("       No processes running.\n");
    else
    {
        iter = iter->next;
        for (; iter != NULL; iter = iter->next)
        {
            printf("    %6d    %10s    0x%08x\n",
                (int)iter->pid,
                state[iter->state],
                iter->exit_status);
        }
    }
}

table_t *allocate_table(void)
{
    table_t *pt = (table_t *)malloc( sizeof(table_t) );

    if(!pt)             // If the malloc failed:
    {
        fprintf(stderr, "Memory allocation failure: %s\n", strerror(errno));
        return NULL;
    }
    pt->children = 0;
    pt->p_tab = NULL;

    return pt;
}

void deallocate_table(table_t *pt)
{
}

int insert_process_table(table_t *pt, pid_t pid)
{

    // First, if this will be the first process in process table pt,
    // allocate new space for the head and add it.
    if(pt->p_tab == NULL)
    {
        pt->p_tab = (child_t *)malloc( sizeof(child_t) );
        pt->children++;

        if(!(pt->p_tab)) // Check for validity:
        {
            return -1;
        }

        if(pt->p_tab == NULL)
            printf("it's null!\n");
        else
            printf("not null!\n");

        pt->p_tab->next = (child_t *)malloc( sizeof(child_t) );
        pt->p_tab->next->pid = pid;
        pt->p_tab->next->state = STATE_RUNNING;
        pt->p_tab->next->exit_status = 0;
    }

    // If data already exists in the process table (with head p_tab), go to
    // end of process table, then add our new data.
    else
    {
        child_t *iter = pt->p_tab;
        int count = 0;
        for(; iter->next != NULL; iter = iter->next)
            count++;
        printf("count: %d\n", count);
        
        //iter = iter->next;
        iter->next = (child_t *)malloc( sizeof(child_t) );

        if(!iter->next) // Check for validity:
        {
            return -1;
        }
        printf("not failed yet?\n");

        iter->next->pid = pid;
        iter->next->state = STATE_RUNNING;
        iter->next->exit_status = 0;
        iter->next->next = NULL;
    }

    if (pr7_debug) print_process_table(pt, __func__);

    return 0;
}


int update_process_table(table_t *pt, pid_t pid, int status)
/* exit status from completed process */
{

    if (pr7_debug) print_process_table(pt, __func__);
}


int remove_process_table(table_t *pt, pid_t pid)
{

    if (pr7_debug) print_process_table(pt, __func__);
}


/*----------------------------------------------------------------------------*/
