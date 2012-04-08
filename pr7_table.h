#ifndef PR7_TABLE_H
#define PR7_TABLE_H

/*
 * pr7_table.h - function declarations for process table
 * Erich Stoekl, CMPSC 311, SP 12
 *
 */

/*----------------------------------------------------------------------------*/

/* This is a rather primitive implementation.  It should be improved. */

#include <sys/types.h>

typedef struct pr7_process
{
    pid_t pid;                                    /* process ID, supplied from fork() */
/* if 0, this entry is currently not in use */
    int   state;                                  /* process state, your own definition */
    int   exit_status;                            /* supplied from wait() if process has finished */

    struct pr7_process *next;
} child_t;

typedef struct process_table
{
    int children;
    child_t *p_tab;
} table_t;

extern int pr7_debug;

extern int number_of_children(table_t *pt);

extern table_t *allocate_table(void);
extern void deallocate_table(table_t *pt);

extern void print_process_table(table_t *pt, const char * const caller);

extern int insert_process_table(table_t *pt, pid_t pid);			/* new process */
extern int update_process_table(table_t *pt, pid_t pid, int status);	/* exit status from completed process */
extern int remove_process_table(table_t *pt, pid_t pid);			/* defunct process */

/*----------------------------------------------------------------------------*/

#endif
