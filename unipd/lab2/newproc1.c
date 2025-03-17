/**
 * SO Lab 2 - Task 1
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;

	/* fork a child process */
	pid = fork();

	if (pid < 0) { /* error occurred */
		fprintf(stderr, "fork failed\n");
		return 1;
	}
	else if (pid == 0) { /* child process */
		printf("[child] fork returned %d\n",pid);
		/* child will load and execute a different program*/
		execlp("/bin/ls","ls",NULL);
	}
	else { /* parent process */
		usleep(10000); /* wait 10 ms */
		printf("[parent] fork returned %d\n",pid);
		/* parent will wait for the child to complete */
		wait(NULL);
		printf("[parent] child has finished\n");
	}
    
    return 0;
}
