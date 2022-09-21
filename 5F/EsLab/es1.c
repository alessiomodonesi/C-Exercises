// Alessio Modonesi 5F
// Scrivere un programma in C che gestica, attraverso un menù, delle operazione sugli array

#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

void StampaArray(int array[])
{
    printf("Stampa: ");
    for (int i = 0; i < SIZE; i++)
        printf("%d ", array[i]);
}

void StampaInversa(int array[])
{
    printf("Inversa: ");
    for (int i = SIZE - 1; i >= 0; i--)
        printf("%d ", array[i]);
}

void SommaMedia(int array[])
{
    int somma, media;
    for (int i = 0; i < SIZE; i++)
        somma += array[i];
    media = somma / SIZE;
    printf("Somma: %d\n", somma);
    printf("Media: %d", media);
}

void StampaPari(int array[])
{
    printf("Numeri pari: ");
    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] % 2 == 0)
            printf("%d ", array[i]);
    }
}

void StampaDispari(int array[])
{
    printf("Numeri dispari: ");
    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] % 2 != 0)
            printf("%d ", array[i]);
    }
}

void Cerca(int array[])
{
    int trova = 0;
    printf("Digita numero da cercare: ");
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
    int elimina = 0;
    printf("Digita numero da eliminare: ");
    scanf("%d", &elimina);

    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] == elimina)
        {
            array[i] = -1;
            printf("Numero eliminato\n");
            break;
        }
    }
    StampaArray(array);
}

void Alterna(int array[])
{
    int cont;

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

void Ordina(int array[])
{
    printf("Non funzia");

    int cont;

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = SIZE - 1; j >= 0; j--)
        {
            if (array[i] < array[j])
            {
                cont = array[j];
                array[j] = array[i];
                array[i] = cont;
            }
            else if (array[i] > array[j])
            {
                cont = array[i];
                array[i] = array[j];
                array[j] = cont;
            }
            else
                printf("Numeri uguali");
        }
    }
    StampaArray(array);
}

int main()
{
    int array[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int scelta;

    printf("Scegli un menù (da 1 a 9): ");
    scanf("%d", &scelta);

    switch (scelta)
    {
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
    default:
        break;
    }

    return 0;
}