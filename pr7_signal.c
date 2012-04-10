/* CMPSC 311, Spring 2012, Project 7
 *
 * Authors:   Erich Stoekl and Andrew Moyer
 * Email:     ems5311@psu.edu and abm5149@psu.edu
 *
 * pr7_signal.c
 * 
 * Function implementations of signal.h functions
 * Recycled with slight changes from project 6
 *
 */

/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/* for sigaction(2), signal(3C), signal numbers */
#include <signal.h>

#include "pr7_signal.h"

/* The default here is to use sigaction() instead of signal().
 * If you decide to simplify the code by allowing only one choice, then you
 * should use sigaction() instead of signal(), and read CS:APP Sec. 8.5.5
 * or APUE Fig. 10.18.
 */

#ifdef PR7_USE_SIGNAL
#undef PR7_USE_SIGACTION
#else
#define PR7_USE_SIGACTION
#endif

/*----------------------------------------------------------------------------*/

/* return 0 if successful, -1 if not */
int install_signal_handler(int sig, sighandler_t func)
{
  int error;

#ifdef PR7_USE_SIGACTION
  struct sigaction sigact;		/* signal handler descriptor */
  int ret;				/* error indicator */
 
  sigact.sa_handler = func;		/* name of the signal handler function */
  sigemptyset(&sigact.sa_mask);		/* do not mask any interrupts while in handler */
  sigact.sa_flags = SA_RESTART;		/* restart system functions if interrupted */
					/* later experiment - replace SA_RESTART with 0 */
  ret = sigaction(sig, &sigact, NULL);	/* do the installation */
  error = (ret == -1);			/* error condition */
#else
  sighandler_t *ret;			/* previous handler or error indicator */

  ret = signal(sig, func);		/* the old-fashioned non-POSIX way */
  error = (ret == SIG_ERR);		/* error condition */
#endif

  if (error)
    {
      fprintf(stderr, "install_signal_handler(%d) failed: %s\n", sig, strerror(errno));
    }

  return (error ? -1 : 0);
}

// Function to toggling blocking of a signal on or off.
// Useful for blocking SIGINT signal for background processes
int block_signal(int sig, _Bool block)
{
    // Block signal with sigprocmask:
    sigset_t sigint_block;
    sigemptyset(&sigint_block);
    sigaddset(&sigint_block, sig);

    int ret;
    if(block)
        ret = sigprocmask(SIG_BLOCK, &sigint_block, NULL);
    else
        ret = sigprocmask(SIG_UNBLOCK, &sigint_block, NULL);

    return ret;
}
