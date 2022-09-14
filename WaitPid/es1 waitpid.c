//Alessio Modonesi 4F 
//Creare 3 figli che scrivano in sequenza una frsase in un file; il padre visualizza il file al termine del figlio più lento.
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    int status;
    FILE *stream;

    if((stream = fopen("input.txt", "w")) == NULL)
    {
      printf("Errore nell'apertura del file \n");
      exit(0);
    }
    
    printf("Mostro i passaggi \n");
    int pid1 = fork();
    if (pid1 == 0)
    {
      sleep(1);
      printf("2) il 1o figlio ha scritto con pid: %i \n", getpid());
      fprintf(stream, "Ehy, sono il figlio n.1 con pid: %i; ", getpid());
      exit(1);
    }
    else
    {
      int pid2 = fork();
      if (pid2 == 0)
      {
        sleep(2);
        printf("3) il 2o figlio ha scritto con pid: %i \n", getpid());
        fprintf(stream, "Ehy, sono il figlio n.2 con pid: %i; ", getpid());
        exit(2);
      }
        else
        {
          int pid3 = fork();
          if (pid3 == 0)
          {
            sleep(3);
            printf("4) il 3o figlio ha scritto con pid: %i \n", getpid());
            fprintf(stream, "Ehy, sono il figlio n.3 con pid: %i.", getpid());
            exit(3);
          }
            else
            {
              printf("1) il padre è in attesa del figlio più lento\n");
              int pid = waitpid(pid3, &status, 0);
              printf("5) il figlio più lento ha terminato con pid: %d \n\n", pid);
              fclose(stream);
              
              printf("Mostro il contenuto del file \n");
              if((stream = fopen("input.txt", "r")) == NULL)
              {
                printf("Errore nell'apertura del file \n");
                exit(0);
              }

              char str[100][100];
              int nparole = 0;

              while(fscanf(stream, "%s", str[nparole]) != EOF)
                nparole++;
            
              for(int i = 0; i < nparole; i++)
                printf("%s ", str[i]);
                
              fclose(stream);
              return 0;
            }
        }
    }
}
