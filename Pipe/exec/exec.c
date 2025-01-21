//Alessio Modonesi 4F
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int piped[2], status;
    pipe(piped);

    int PID1 = fork();
    if (PID1 < 0)
    {
        printf("Errore nella generazione del figlio: %d\n", getpid());
        exit(1);
    }
    else if (PID1 == 0) 
    {
        close(1); //chiudo lo standard output di PID1
        dup(piped[1]); //duplico il canale write della pipe
        close(piped[1]); //chiudo il canale originale
        close(piped[0]); //chiudo il canale read della pipe

        execl("/bin/cat", "cat", "file.txt", (char* )0); //chiamo la exec, che esegue cat
        exit(0);
    }
    else
    {
        int PID2 = fork();
        if (PID2 < 0)
        {
            printf("Errore nella generazione del figlio: %d\n", getpid());
            exit(1);
        }
        else if (PID2 == 0)
        {
            close(0); //chiudo lo standard input di PID2
            dup(piped[0]); //duplico il canale read della pipe
            close(piped[0]); //chiudo il canale originale
            close(piped[1]); //chiudo il canale write della pipe

            execl("/bin/wc", "wc", (char* )0); //chiamo la exec, che esegue wc
            exit(0);
        }
        else
        {
            wait(&status); 
            close(piped[0]); //chiudo il canale read della pipe
            close(piped[1]); //chiudo il canale write della pipe
        }    
    }
    return 0;
}