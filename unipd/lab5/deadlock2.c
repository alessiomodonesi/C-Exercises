/* C program to demonstrate a deadlock
 * 
 * Compile:
 *	gcc deadlock2.c -o deadlock2 -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t R1, R2, R3;


void* T1_run(void* arg)
{
	/* initialize resources */
	sem_wait(&R2);

	/* do something */
	printf("T1 running...\n");
	sleep(1);
	/* request new resource */
	sem_wait(&R1);

	printf("T1 completed.\n");

	/* release resources */
	sem_post(&R2);
	sem_post(&R1);

	pthread_exit(0);
}


void* T2_run(void* arg)
{
	/* initial resources */
	sem_wait(&R1);
	sem_wait(&R2);

	/* do something */
	printf("T2 running...\n");
	sleep(1);
	/* request new resource */
	sem_wait(&R3);

	printf("T2 completed.\n");

	/* release resources */
	sem_post(&R3);
	sem_post(&R2);
	sem_post(&R1);

	pthread_exit(0);
}


void* T3_run(void* arg)
{
	/* initial resources */
	sem_wait(&R3);

	/* do something */
	printf("T3 running...\n");
	sleep(1);
	/* request new resource */
	sem_wait(&R2);

	printf("T3 completed.\n");

	/* release resources */
	sem_post(&R2);
	sem_post(&R3);

	pthread_exit(0);
}


int main()
{

	pthread_t T1, T2, T3;

	/* Initialize resource semaphores */
	sem_init(&R1, 0, 1);
	sem_init(&R2, 0, 2);
	sem_init(&R3, 0, 1);

	/* Create threads */
	pthread_create(&T1, NULL, T1_run, NULL);
	pthread_create(&T2, NULL, T2_run, NULL);
	pthread_create(&T3, NULL, T3_run, NULL);
	
	/* Wait for threads to finish */
	pthread_join(T1, NULL);
	pthread_join(T2, NULL);
	pthread_join(T3, NULL);

	/* Destroy semaphores */
	sem_destroy(&R1);
	sem_destroy(&R2);
	sem_destroy(&R3);

	return 0;
}
