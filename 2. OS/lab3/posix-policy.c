/**
 * A simple pthread program illustrating pthread scheduling policy.
 *
 * To compile:
 *
 *	gcc posix-policy.c -o posix-policy -lpthread
 */

#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS 5

/* the thread runs in this function */
void *runner(void *param); 

int main(int argc, char *argv[])
{
	int i, policy;
	pthread_t tid[NUM_THREADS]; 	/* the thread identifier */
	pthread_attr_t attr; 		/* set of attributes for the thread */

	/* get the default attributes */
	pthread_attr_init(&attr);

	/* get the current scheduling policy */
	if (pthread_attr_getschedpolicy(&attr, &policy) != 0)
		fprintf(stderr, "Unable to get policy.\n");
	else {
		if (policy == SCHED_OTHER)
			printf("SCHED_OTHER\n");
		else if (policy == SCHED_RR)
			printf("SCHED_RR\n");
		else if (policy == SCHED_FIFO)
			printf("SCHED_FIFO\n");
	}
	
	if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) != 0)
		printf("Unable to set explicit scheduling policy\n");
	/* set the scheduling policy - FIFO, RR, or OTHER */
	if (pthread_attr_setschedpolicy(&attr, SCHED_OTHER) != 0)
		printf("Unable to set desired scheduling policy to SCHED_OTHER\n");

	/* create the threads */
	for (i = 0; i < NUM_THREADS; i++) 
		pthread_create(&tid[i], &attr, runner, NULL); 

	/**
	 * Now join on each thread
	 */
	for (i = 0; i < NUM_THREADS; i++) 
		pthread_join(tid[i], NULL);
}


static int counter = 0;

/**
 * The thread will begin control in this function.
 */
void *runner(void *param) 
{
	/* do some work ... */
	printf("counter = %d\n", counter++);

	pthread_exit(0);
}
