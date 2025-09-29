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
	pid_t pidA, pidB;

	sem_t* psem = sem_open("/psem", O_CREAT, 0666, 0);
	sem_t* csem = sem_open("/csem", O_CREAT, 0666, 2);

	/* Create child process*/
	pidA = fork();
	
	if (pidA == 0)
	{	/* this is the child process*/
		sem_wait(csem);
		printf("This is producer A\n");
		sleep(1);
		sem_post(psem);
		/* terminate the child process*/
		exit(0);
	}

	/* Create child process*/
	pidB = fork();
	
	if (pidB == 0)
	{	/* this is the child process*/
		sem_wait(csem);
		printf("This is producer B\n");
		sleep(1);
		sem_post(psem);
		/* terminate the child process*/
		exit(0);
	}

	sem_wait(psem);
	sem_wait(psem);
	printf("This is consumer\n");
	sleep(1);
	
	/* Wait for any child process to finish */
	wait(NULL);

	/* Close and remove semaphores */
	sem_close(psem);
	sem_close(csem);
	sem_unlink("/psem");
	sem_unlink("/csem");

	return 0;
}
