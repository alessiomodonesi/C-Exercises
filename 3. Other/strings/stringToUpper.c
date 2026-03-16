#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("errore nell'inserimento degli argomenti\n");
        exit(1);
    }
    else
    {
        int PID = fork();
        if (PID < 0)
        {
            printf("errore nella generazione del processo figlio\n");
            exit(2);
        }
        else if (PID == 0) //processo figlio
        {
            printf("Figlio: Inizio la conversione dei caratteri in minuscolo a maiuscolo\n");
            int dim = strlen(argv[1]);
            for (int i = 0; i < dim; i++)
                argv[1][i] = toupper(argv[1][i]);
            printf("Figlio: %s\n", argv[1]);
            printf("Figlio: Ho terminato\n");
            exit(3);
        }
        else //processo padre
        {
            wait(&PID);
            printf("Padre: Il processo figlio ha terminato\n");
        }
    }

    return 0;
}