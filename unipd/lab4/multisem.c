/* C program to demonstrate working of semaphores
 * 
 * Compile:
 *	gcc multisem.c -o multisem -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t sem1, sem2, sem3;


void* reader_run(void* arg)
{
	while (1)
	{
		printf("This is reader.\n");
		sleep(1);
		sem_post(&sem1);
	}
}


void* sorter_run(void* arg)
{
	while (1)
	{
		sem_wait(&sem1);
		printf("This is sorter.\n");
		sleep(1);
		sem_post(&sem2);
	}
}


void* writer_run(void* arg)
{
	while (1)
	{
		sem_wait(&sem1);
		sem_wait(&sem2);
		printf("This is writer.\n\n");
		sleep(1);
		sem_post(&sem3);
	}
}


int main()
{

	pthread_t reader, sorter, writer;
	
	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 0);
	sem_init(&sem3, 0, 0);

	/* Create threads */
	pthread_create(&reader, NULL, reader_run, NULL);
	pthread_create(&sorter, NULL, sorter_run, NULL);
	pthread_create(&writer, NULL, writer_run, NULL);
	
	/* Wait for threads to finish */
	pthread_join(reader, NULL);
	pthread_join(sorter, NULL);
	pthread_join(writer, NULL);

	/* Destroy semaphores */
	sem_destroy(&sem1);
	sem_destroy(&sem2);
	sem_destroy(&sem3);

	return 0;
}
