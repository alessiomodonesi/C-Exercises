/* C program to demonstrate working of semaphores
 * 
 * Compile:
 *	gcc procsem.c -o procsem -lpthread
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
	pid_t pidA;

	sem_t* psem = sem_open("/psem", O_CREAT, 0666, 0);
	sem_t* csem = sem_open("/csem", O_CREAT, 0666, 1);

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
