/* C program to demonstrate working of semaphores
 * 
 * Compile:
 *	gcc posix-sem.c -o posix-sem -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t sem; /* semaphore */


void* thread(void* arg)
{
	char label = *(char*)arg;
	printf("Started thread %c\n", label);

	sem_wait(&sem); /* wait */
	
	printf("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
	printf("Entered %c's critical region...\n", label);

	/* Critical section */
	sleep(4); /* this suspends the thread for a few seconds... */
	
	printf("Exiting %c's critical region.\n", label);
	printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	
	sem_post(&sem); /* signal */

	/* Remainder section */
	sleep(2);

	printf("End of thread %c\n", label);
}


int main()
{

	pthread_t t1, t2;
	char label1 = 'A';
	char label2 = 'B';
	
	/* Initialise semaphore:
	 *  0 means this is shared among threads in the same process
	 *  1 is the initial value assigned to the semaphore
	 * See "man sem_init" for more information.
	 */
	sem_init(&sem, 0, 1);

	/* Create threads */
	pthread_create(&t1, NULL, thread, &label1);
	sleep(2);
	pthread_create(&t2, NULL, thread, &label2);
	
	/* Wait for threads to finish */
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	/* Destroy semaphore */
	sem_destroy(&sem);

	return 0;
}
