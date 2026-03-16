#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct
{
    char nome[10];
    char cognome[10];
    char ruolo[14];
} GIOCATORE_T;

int ContaRighe(FILE *stream, char filename[])
{
    if ((stream = fopen(filename, "r")) == NULL)
    {
        printf("Impossibile leggere il file\n");
        exit(1);
    }

    int righe = 0;
    char tmp;

    while (!feof(stream)) //conto le righe
    {
        tmp = fgetc(stream);
        if (tmp == '\n')
            righe++;
    }

    if (tmp == EOF && righe != 0) //aggiungo una riga finale
        righe++;

    fclose(stream);
    return righe;
}

void LetturaFile(FILE *stream, char filename[], int righe, GIOCATORE_T calciatori[])
{
    if ((stream = fopen(filename, "r")) == NULL)
    {
        printf("Impossibile leggere il file\n");
        exit(1);
    }

    GIOCATORE_T squadra[righe];

    for (int i = 0; !feof(stream); i++) //leggo nome, cognome e ruolo di ogni giocatore
    {
        fscanf(stream, "%s %s %s", squadra[i].nome, squadra[i].cognome, squadra[i].ruolo);
        strcpy(calciatori[i].nome, squadra[i].nome);
        strcpy(calciatori[i].cognome, squadra[i].cognome);
        strcpy(calciatori[i].ruolo, squadra[i].ruolo);
    }

    fclose(stream);
}

void MostraAttacanti(GIOCATORE_T output[], int length)
{
    for (int i = 0; i < length; i++) //stampo solo gli attaccanti
        printf("%s %s\n", output[i].nome, output[i].cognome);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("argomenti errati\n");
        exit(1);
    }

    FILE *fp;
    int righe = ContaRighe(fp, argv[1]);
    GIOCATORE_T giocatori[righe];

    LetturaFile(fp, argv[1], righe, giocatori);

    int attaccanti = 0;

    for (int i = 0; i < righe; i++) //conto gli attaccanti
    {
        if (strcmp(giocatori[i].ruolo, "attaccante") == 0)
            attaccanti++;
    }

    GIOCATORE_T _attaccanti[attaccanti];
    int tmp = 0;

    /*for (int i = 0; i < righe; i++)
    {
        if (strcmp(giocatori[i].ruolo, "attaccante") == 0)
        {
            strcpy(_attaccanti[tmp].nome, giocatori[i].nome);
            strcpy(_attaccanti[tmp].cognome, giocatori[i].cognome);
            tmp++;
        }
    }*/

    int PID = fork();
    if (PID < 0)
    {
        printf("Errore con la generazione del processo figlio\n");
        exit(1);
    }
    else if (PID == 0)
    {
        printf("Figlio: Mostro gli attaccanti\n");
        MostraAttacanti(_attaccanti, attaccanti);
        exit(1);
    }
    else if (PID > 0)
    {
        int status;
        wait(&status);
        printf("Padre: Il processo figlio ha terminato con stato: %d\n", status);
    }
    return 0;
}