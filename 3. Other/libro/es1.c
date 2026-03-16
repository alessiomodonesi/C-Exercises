//Alessio Modonesi 4F
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int status;
    int pid = fork();
    if (pid == 0)
    {
        printf("Sono il figlio e questo è il mio PID: %d\n", getpid());
        exit(0);
    }
    else if (pid > 0)
    {
        printf("Sono il padre e questo è il mio PID: %d\n", getpid());
        wait(&status);
        printf("Il figlio ha terminato\n");
    }
    return 0;
}