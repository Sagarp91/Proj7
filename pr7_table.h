#ifndef PR7_TABLE_H
#define PR7_TABLE_H

/* CMPSC 311, Spring 2012, Project 7
 *
 * Authors:   Erich Stoekl and Andrew Moyer
 * Email:     ems5311@psu.edu and abm5149@psu.edu
 *
 * pr7_table.h
 * 
 * Definitions of node structs necessary for process table,
 * and declarations of functions which build and use a linked list
 * of processes from said structs.
 *
 * In these comments, 'process' is used to describe 'linked list node'
 */

/*----------------------------------------------------------------------------*/


#include <sys/types.h>

// Both structs are typedef so it's not necessary to type 'struct'
// every time you want to make a new struct object.
//
// Node struct for a typical process node:
typedef struct pr7_process
{
    pid_t pid;                  /* process ID, supplied from fork() */
                                /* if 0, this entry is currently not in use */
    int   state;                /* process state, your own definition */
    int   exit_status;          /* supplied from wait() if process has finished */

    struct pr7_process *next;
} child_t;

// Struct to manage the process table and keep count of the number
// of active children:
typedef struct process_table
{
    int children;
    child_t *p_tab;
} table_t;

extern int pr7_debug;

// Returns number of children allocated
extern int number_of_children(table_t *pt);

// Allocate new table
// Returns new table_t
extern table_t *allocate_table(void);
// Delete the table pt
extern void deallocate_table(table_t *pt);

// Print out the full process table pt
extern void print_process_table(table_t *pt, const char * const caller);

// Insert a new child process into the process table pt
extern int insert_process_table(table_t *pt, pid_t pid);

// Updates the state of an existing process
extern int update_process_table(table_t *pt, pid_t pid, int status);

// Removes a process from the table
extern int remove_process_table(table_t *pt, pid_t pid);

/*----------------------------------------------------------------------------*/

#endif
