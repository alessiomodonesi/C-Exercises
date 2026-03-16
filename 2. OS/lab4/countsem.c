/* C program to demonstrate working of semaphores
 * 
 * Compile:
 *	gcc countsem.c -o countsem -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t csem, psem;


void* producerA_run(void* arg)
{
	while (1)
	{
		sem_wait(&csem);
		printf("This is producer A.\n");
		sem_post(&psem);
		sleep(3);
	}
}


void* producerB_run(void* arg)
{
	while (1)
	{
		sem_wait(&csem);
		printf("This is producer B.\n");
		sem_post(&psem);
		sleep(1);
	}
}


void* consumer_run(void* arg)
{
	while (1)
	{
		sem_wait(&psem);
		sem_wait(&psem);
		printf("This is consumer.\n\n");
		sleep(1);
		sem_post(&csem);
		sem_post(&csem);
	}
}


int main()
{

	pthread_t producerA, producerB, consumer;
	
	sem_init(&psem, 0, 0);
	sem_init(&csem, 0, 2);

	/* Create threads */
	pthread_create(&producerA, NULL, producerA_run, NULL);
	pthread_create(&producerB, NULL, producerB_run, NULL);
	pthread_create(&consumer, NULL, consumer_run, NULL);
	
	/* Wait for threads to finish */
	pthread_join(producerA, NULL);
	pthread_join(producerB, NULL);
	pthread_join(consumer, NULL);

	/* Destroy semaphores */
	sem_destroy(&psem);
	sem_destroy(&csem);

	return 0;
}
