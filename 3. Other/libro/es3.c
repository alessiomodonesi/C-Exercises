//Alessio Modonesi 4F
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


int main()
{
    int status;
    int pid1 = fork();
    if (pid1 == 0)
    {
        printf("Ciao, io sono Qui\n");
        exit(1);
    }
    else if (pid1 > 0)
    {
        int pid2 = fork();
        if (pid2 == 0)
        {
            printf("Ciao, io sono Quo\n");
            exit(2);
        }
        else if (pid2 > 0)
        {
            int pid3 = fork();
            if (pid3 == 0) 
            {
                printf("Ciao, io sono Qua\n");
                exit(3);
            }
            else if (pid3 > 0)
            {
                waitpid(pid3, &status, 0);
                printf("L'ultimo figlio ha terminato\n");
            }
        }
        return 0;
    }
}