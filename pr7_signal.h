#ifndef PR6_SIGNAL_H
#define PR6_SIGNAL_H

/* CMPSC 311 Project 6 toolkit 
 *
 * This file is provided as part of the project description and starter kit.
 * If you modify it, then put your name, email address, and the date, in this
 * comment, and include this file in the materials you turn in.
 */

/*----------------------------------------------------------------------------*/

#include <signal.h>

typedef void sighandler_t(int);
    /* the type of a function that has one int argument, and returns nothing */

int install_signal_handler(int sig, sighandler_t func, _Bool ignore);
    /* returns 0 if successful, -1 if not */

int block_signal(int sig, _Bool block);

void reinstall_signal_handler(int sig, sighandler_t func);

/*----------------------------------------------------------------------------*/

#endif
