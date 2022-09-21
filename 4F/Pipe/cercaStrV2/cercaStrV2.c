//Alessio Modonesi 4F
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define MAX 100

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        int status, totale = 0;
        char str[MAX];
        int P1P2[2], P2P3[2], P3P0[2];

        while (1)
        {
            printf("Inserisci la parola che vuoi cercare: ");
            scanf("%s", str);
            if (strcmp(str, "fine") != 0)
            {
                pipe(P1P2);
                int P1 = fork();
                if (P1 == 0)
                {
                    close(1);
                    dup(P1P2[WRITE]);
                    close(P1P2[WRITE]);

                    execl("/bin/cat", "cat", argv[1], NULL);
                    exit(0);
                }
                else
                {
                    pipe(P2P3);
                    int P2 = fork();
                    if (P2 == 0)
                    {
                        close(0);
                        dup(P1P2[READ]);
                        close(P1P2[READ]);

                        close(1);
                        dup(P2P3[WRITE]);
                        close(P2P3[WRITE]);

                        execl("/bin/grep", "grep", "-o", str, NULL);
                        exit(0);

                    }
                    else
                    {
                        pipe(P3P0);
                        int P3 = fork();
                        if (P3 == 0)
                        {   
                            close(0);
                            dup(P2P3[READ]);
                            close(P2P3[READ]);

                            close(1);
                            dup(P3P0[WRITE]);
                            close(P3P0[WRITE]);
                        
                            execl("/bin/wc", "wc", "-l", NULL);
                            exit(0);
                        } 
                        else
                        {
                            wait(&status);
                            char cont[MAX];
                            read(P3P0[READ], &cont, sizeof(cont));
                            printf("'%s' è stata individuata %d volte\n", str, atoi(cont));
                            totale += atoi(cont);
                        }
                    }
                }
            }
            else 
                break;  
        }
        printf("Ho trovato %d parole\n", totale);
    }
    else
        printf("Argomenti errati\n");
    return 0;
}