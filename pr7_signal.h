#ifndef PR6_SIGNAL_H
#define PR6_SIGNAL_H

/* CMPSC 311, Spring 2012, Project 7
 *
 * Authors:   Erich Stoekl and Andrew Moyer
 * Email:     ems5311@psu.edu and abm5149@psu.edu
 *
 * pr7_signal.h
 * 
 * Declarations of functions to set up signal handling
 * 
 */

/*----------------------------------------------------------------------------*/

#include <signal.h>

typedef void sighandler_t(int);
    /* the type of a function that has one int argument, and returns nothing */

int install_signal_handler(int sig, sighandler_t func);
    /* returns 0 if successful, -1 if not */

// Function for switching blocking of a signal on or off
int block_signal(int sig, _Bool block);

/*----------------------------------------------------------------------------*/

#endif
