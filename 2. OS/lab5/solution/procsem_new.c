/* C program to demonstrate working of semaphores
 *
 * Compile:
 *	gcc procsem_new.c -o procsem_new -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int main()
{

	pid_t numA, numB;

	sem_t *psem = sem_open("/psem", O_CREAT, 0666, 0);
	sem_t *csem = sem_open("/csem", O_CREAT, 0666, 2);

	/* Create child process*/
	numA = fork();

	if (numA == 0)
	{
		/* this is the child process*/
		sem_wait(csem);
		printf("This is the PRODUCER child A\n");
		sleep(2);
		sem_post(psem);
		/* terminate the child process*/
		exit(0);
	}

	/* Create child process*/
	numB = fork();

	if (numB == 0)
	{
		/* this is the child process*/
		sem_wait(csem);
		printf("This is the PRODUCER child B\n");
		sleep(2);
		sem_post(psem);
		/* terminate the child process*/
		exit(0);
	}

	sem_wait(psem);
	sem_wait(psem);
	printf("This is the CONSUMER parent\n");
	sleep(2);
	sem_post(csem);

	/* Wait for any child process to finish */
	wait(NULL);

	/* Close and remove semaphores */
	sem_close(psem);
	sem_close(csem);
	sem_unlink("/psem");
	sem_unlink("/csem");

	return 0;
}
