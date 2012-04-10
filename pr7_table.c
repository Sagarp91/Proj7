/*----------------------------------------------------------------------------*/

/* CMPSC 311, Spring 2012, Project 7
 *
 * Authors:   Erich Stoekl and Andrew Moyer
 * Email:     ems5311@psu.edu and abm5149@psu.edu
 *
 * pr7_table.c
 * 
 * Implementations of functions declared in pr7_table.c
 * Adapted from pr6_table functions from project 6, but
 * rebuilt to use a linked list for dynamic allocation of
 * new table entries.
 *
 * Can insert an indefinite amount of process entries into table
 * (restriction would be the amount of memory program is allowed to use)
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "pr7_table.h"

/* fixed-size process table, give the size as a symbolic constant */
#define MAX_CHILDREN 8

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
    return pt->children;
}

void print_process_table(table_t *pt, const char * const caller)
{
    // Create an iterator object:
    child_t *iter = pt->p_tab;

    printf("  in pid %d, process table, called from %s\n", getpid(), caller);
    printf("       pid         state        status\n");

    if (iter == NULL);
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

// Allocate a new process table
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
    // Free the process table.
    free(pt);
}

// Insert a new node into the process table.
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

        pt->p_tab->next = (child_t *)malloc( sizeof(child_t) );
        if(!(pt->p_tab->next))
            return -1;

        pt->p_tab->next->pid = pid;
        pt->p_tab->next->state = STATE_RUNNING;
        pt->p_tab->next->exit_status = 0;
    }

    // If data already exists in the process table (with head p_tab), go to
    // end of process table, then add our new data.
    else
    {
        pt->children++;

        child_t *iter = pt->p_tab;
        for(; iter->next != NULL; iter = iter->next);
        
        //iter = iter->next;
        iter->next = (child_t *)malloc( sizeof(child_t) );

        if(!iter->next) // Check for validity:
        {
            return -1;
        }

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
    // Iterate through table to find matching pid
    child_t *iter = pt->p_tab;

    for(; iter != NULL; iter = iter->next)
    {
        if(iter->pid == pid)
            iter->state = status;
    }

    if (pr7_debug) print_process_table(pt, __func__);

    return 0;
}


int remove_process_table(table_t *pt, pid_t pid)
{
    // Find the process with the given pid and free it from memory:
    child_t *iter = pt->p_tab;
    
    for(; iter != NULL; iter = iter->next)
    {
        if(iter->next->pid == pid)
        {
            // Then we need to kill iter->next
            child_t *temp = iter->next->next;
            free(iter->next);
            iter->next = temp;

            (pt->children)--;

            if (pr7_debug) print_process_table(pt, __func__);
            return 0;
        }
    }

    printf("remove_process_table: target pid not found.\n");
    return -1;

}


/*----------------------------------------------------------------------------*/
