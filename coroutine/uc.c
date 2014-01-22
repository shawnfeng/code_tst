#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
     
/* Set by the signal handler. */
static volatile int expired;
     
/* The contexts. */
static ucontext_t uc[3];
     
/* We do only a certain number of switches. */
static int switches;
     
     
/* This is the function doing the work.  It is just a
   skeleton, real code has to be filled in. */
static void f(int n)
{
	int m = 0;
	while (1) {
		/* This is where the work would be done. */
		if (++m % 100 == 0) {
			putchar ('.');
			fflush (stdout);
		}
     
		/* Regularly the expire variable must be checked. */
		if (expired) {
			printf("\nSWITCH COUNT %d\n", switches);
			/* We do not want the program to run forever. */
			if (++switches == 3)
				return;
     
			printf ("\nswitching %d from %d to %d\n", switches, n, 3 - n);
			expired = 0;
			/* Switch to the other context, saving the current one. */
			int rv = swapcontext (&uc[n], &uc[3 - n]);
			printf ("OVER %d %d switching from %d to %d\n", switches, rv, n, 3 - n);
		}
	}
}
     
/* This is the signal handler which simply set the variable. */
void handler(int signal)
{
	expired = 1;
}
     
#define STACK_SIZE 8192
int main(void)
{
	
	struct sigaction sa;
	struct itimerval it;
	//char st1[STACK_SIZE];
	//char st2[STACK_SIZE];
	char *st1 = malloc(STACK_SIZE);
	char *st2 = malloc(STACK_SIZE);

     
	/* Initialize the data structures for the interval timer. */
	sa.sa_flags = SA_RESTART;
	sigfillset (&sa.sa_mask);
	sa.sa_handler = handler;
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 1;
	it.it_value = it.it_interval;
     
	/* Install the timer and get the context we can manipulate. */
	if (sigaction (SIGPROF, &sa, NULL) < 0
	    || setitimer (ITIMER_PROF, &it, NULL) < 0
	    || getcontext (&uc[1]) == -1
	    || getcontext (&uc[2]) == -1)
		abort ();
     
	/* Create a context with a separate stack which causes the
	   function f to be call with the parameter 1.
	   Note that the uc_link points to the main context
	   which will cause the program to terminate once the function
	   return. */
	uc[1].uc_link = &uc[0];
	uc[1].uc_stack.ss_sp = st1;
	uc[1].uc_stack.ss_size = STACK_SIZE;
	makecontext (&uc[1], (void (*) (void)) f, 1, 1);
     
	/* Similarly, but 2 is passed as the parameter to f. */
	uc[2].uc_link = &uc[0];
	uc[2].uc_stack.ss_sp = st2;
	uc[2].uc_stack.ss_size = STACK_SIZE;
	makecontext (&uc[2], (void (*) (void)) f, 1, 2);
     
	/* Start running. */
	int rv = swapcontext (&uc[0], &uc[1]);
	printf("RV %d\n", rv);

	puts("HERE");
	return 0;
}
