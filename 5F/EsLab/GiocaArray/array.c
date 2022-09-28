// Alessio Modonesi 5F
// Scrivere un programma in C che gestica, attraverso un menù, delle operazione sugli array

#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

void StampaArray(int array[])
{
    printf("\nStampa: ");
    for (int i = 0; i < SIZE; i++)
        printf("%d ", array[i]);
}

void StampaInversa(int array[])
{
    printf("\nInversa: ");
    for (int i = SIZE - 1; i >= 0; i--)
        printf("%d ", array[i]);
}

void SommaMedia(int array[])
{
    double somma, media;
    for (int i = 0; i < SIZE; i++)
        somma += array[i];

    media = somma / SIZE;
    printf("\nSomma: %.2lf", somma);
    printf("\nMedia: %.2lf", media);
}

void StampaPari(int array[])
{
    printf("\nNumeri pari: ");
    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] % 2 == 0)
            printf("%d ", array[i]);
    }
}

void StampaDispari(int array[])
{
    printf("\nNumeri dispari: ");
    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] % 2 != 0)
            printf("%d ", array[i]);
    }
}

void Cerca(int array[])
{
    int trova = 0;
    printf("\nDigita numero da cercare: ");
    scanf("%d", &trova);

    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] == trova)
        {
            printf("Numero trovato in posizione: %d ", array[i]);
            break;
        }
    }
}

void Elimina(int array[])
{
    int elimina = 0, tmp;
    printf("\nDigita numero da eliminare: ");
    scanf("%d", &elimina);

    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] == elimina)
        {
            tmp = array[i];
            array[i] = array[SIZE - 1];
            array[SIZE - 1] = tmp;
            printf("Numero eliminato\n");
            break;
        }
    }
    printf("\nStampa: ");
    for (int i = 0; i < (SIZE - 1); i++)
        printf("%d ", array[i]);
}

void Alterna(int array[])
{
    int cont;
    if (SIZE % 2 == 0)
    {
        for (int i = 0; i < SIZE; i++)
        {
            if (i % 2 == 0)
            {
                cont = array[i + 1];
                array[i + 1] = array[i];
                array[i] = cont;
            }
        }
        StampaArray(array);
    }
    else
        printf("L'array è di dimensione dispari");
}

void Ordina(int array[])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = (i + 1); j < SIZE; j++)
        {
            if (array[i] > array[j])
            {
                int tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }
    }
    StampaArray(array);
}

int main()
{
    int array[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int scelta = 0, esci = 0;

    do
    {
        printf("\n\nScegli un giochino:\
        \n'1' per stampa\
        \n'2' per stampa inversa\
        \n'3' per somma/media\
        \n'4' per stampa pari\
        \n'5' per stampa dispari\
        \n'6' per cerca\
        \n'7' per elimina\
        \n'8' per alterna\
        \n'9' per ordina\
        \n'0' per uscire\
        \n--> ");
        scanf("%d", &scelta);

        switch (scelta)
        {
        case 0:
            // esci
            esci = 1; // setto il flag a true
            break;
        case 1:
            // Stampa degli elementi array
            StampaArray(array);
            break;
        case 2:
            // Stampa inversa degli elementi dell’array
            StampaInversa(array);
            break;
        case 3:
            // Somma e media degli elementi del vettore
            SommaMedia(array);
            break;
        case 4:
            // Stampa tutti i numeri pari
            StampaPari(array);
            break;
        case 5:
            // Stampa tutti i numeri dispari
            StampaDispari(array);
            break;
        case 6:
            // Ricerca di un numero nell’array (letto in input)
            Cerca(array);
            break;
        case 7:
            // Eliminare un elemento dell’array (se esistente)
            Elimina(array);
            break;
        case 8:
            // Alternare (o scambiare ) a due a due le posizioni del vettore
            Alterna(array);
            break;
        case 9:
            // Ordinamento del vettore (ordinamento a scelta)
            Ordina(array);
            break;
        }
    } while (esci == 0);

    return 0;
}