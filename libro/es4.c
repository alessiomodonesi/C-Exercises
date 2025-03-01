//Alessio Modonesi 4F
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


void CambiaChar(char string[])
{
    int dim = strlen(string);
    string[0] = toupper(string[0]);

    for (int i = 1; i < dim; i++)
        string[i] = tolower(string[i]);
}

int main()
{
    FILE *stream;
    char nome[10];
    char cognome[10];

    if ((stream = fopen("output.txt", "w+")) == NULL)
    {
        printf("Impossibile accedere al file\n");
        exit(2);
    }
 
    printf("Scrivi un nome\n");
    scanf("%s", nome);
    printf("Scrivi un cognome\n");
    scanf("%s", cognome);

    int PID = fork();
    if (PID == 0) 
    {
        sleep(5);
        printf("Figlio: Leggo quello che il padre ha scritto\n");
        if ((stream = fopen("output.txt", "r")) == NULL)
        {
            printf("Figlio: Impossibile accedere al file\n");
            exit(2);
        }
        fscanf(stream, "%s %s", nome, cognome);
        printf("Figlio: %s %s\n", nome, cognome);
        printf("Figlio: Ho terminato\n");
        exit(1);
    }
    else
    {
        CambiaChar(nome);
        CambiaChar(cognome);
        fprintf(stream, "%s %s\n", nome, cognome);
        fclose(stream);

        if (remove("output.txt") == 0)
            printf("Padre: Il file è stato eliminato con successo\n");
        else
            printf("Padre: Il file non è stato eliminato\n");

        wait(&PID);
        printf("Padre: Il processo figlio ha terminato\n");
    }
    return 0;
}