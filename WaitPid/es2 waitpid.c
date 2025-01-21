//Alessio Modonesi 4F
//Creare un processo padre, che crea F1 e F2 e ne attende la terminazione in ordine inverso.
//Ciascun figlio avrà a sua volta altri 2 figli: G1, H1 e G2, H2. I rispettivi padri ne aspettaranno la terminazione in ordine di creazione.
#include <stdio.h>
#include <stdlib.h>
//#include <sys/wait.h>
//#include <unistd.h>

int main()
{
    int status;
    srand(time(NULL));

    int F1 = fork();
    if (F1 == 0)
    {
        printf("1) sono il 1o figlio con pid: %i \n", getpid());
        sleep(3);
        exit(1);
    }
    else
    {
        int F2 = fork();
        if (F2 == 0)
        {
            printf("2) sono il 2o figlio con pid: %i \n", getpid());
            sleep(2);
            exit(2);
        }
        else
        {
            printf("4) sono il padre in attesa del figlio più lento \n");
            int pid = waitpid(F1, &status, 0);
            printf("5) il figlio più lento ha terminato con pid: %d \n", pid);
            return 0;
        }
    }   
}