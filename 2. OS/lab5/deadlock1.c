/* C program to demonstrate a deadlock
 * 
 * Compile:
 *	gcc deadlock1.c -o deadlock1 -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t sem1, sem2;


void* threadA(void* arg)
{
	sem_wait(&sem1);
	
	printf("[A] First critical section...\n");
	sleep(1);
	
	sem_wait(&sem2);
	
	printf("[A] Second critical section...\n");
	sleep(1);

	sem_post(&sem2);
	sem_post(&sem1);

	pthread_exit(0);
}


void* threadB(void* arg)
{
	sem_wait(&sem2);
	
	printf("[B] First critical section...\n");
	sleep(1);

	sem_wait(&sem1);
	
	printf("[B] Second critical section...\n");
	sleep(1);
	
	sem_post(&sem1);
	sem_post(&sem2);

	pthread_exit(0);
}


int main()
{

	pthread_t tA, tB;

	/* Initialize semaphores */
	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 1);

	/* Create threads */
	pthread_create(&tA, NULL, threadA, NULL);
	pthread_create(&tB, NULL, threadB, NULL);
	
	/* Wait for threads to finish */
	pthread_join(tA, NULL);
	pthread_join(tB, NULL);

	/* Destroy semaphores */
	sem_destroy(&sem1);
	sem_destroy(&sem2);

	return 0;
}
