/* CMPSC 311 Project 6 toolkit 
 *
 * This file is provided as part of the project description and starter kit.
 * If you modify it, then put your name, email address, and the date, in this
 * comment, and include this file in the materials you turn in.
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
int install_signal_handler(int sig, sighandler_t func, _Bool ignore)
{
  int error;

#ifdef PR7_USE_SIGACTION
  struct sigaction sigact;		/* signal handler descriptor */
  int ret;				/* error indicator */
 
  if(!ignore)
      sigact.sa_handler = func;		/* name of the signal handler function */
  else
      sigact.sa_handler = SIG_IGN;
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

void reinstall_signal_handler(int sig, sighandler_t func)
{
#ifdef PR7_USE_SIGACTION
  /* the signal handler remains installed after a signal is received */
  return;
#else
  /* the default signal handler was reinstalled after a signal is received */
  signal(sig, func);
#endif
}

/*----------------------------------------------------------------------------*/

