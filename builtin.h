/* CMPSC 311, Spring 2012, Project 7
 *
 * Authors:   Erich Stoekl and Andrew Moyer
 * Email:     ems5311@psu.edu and abm5149@psu.edu
 *
 * builtin.h
 * 
 * Function declarations for functions to handle
 * built-in commands to shell
 *
 */
#include "pr7_table.h"

int builtin(char *_argv[], table_t *pt);

int Echo(char *_argv[]);
