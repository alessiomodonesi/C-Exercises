// Alessio Modonesi
/*
Realizzare "array auto-incrementante" di interi, che sia sempre pieno.
Dovrà avere le seguenti funzionalità:
- inserisci numero
- verifica numero
- elimina numero
- ordina
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Inserisci(int array[], int size)
{
    array = realloc(array, (size + 1) * sizeof(int));
    printf("Inserisci un numero: ");
    scanf("%d", &array[size + 1]);
    return size + 1;
}

int Verifica(int array[], int size)
{
    int trova = 0;
    printf("\nDigita numero da cercare: ");
    scanf("%d", &trova);

    for (int i = 0; i <= size; i++)
    {
        if (array[i] == trova)
        {
            printf("Numero trovato in posizione: %d ", array[i]);
            break;
        }
    }
    return size + 1;
}

int Elimina(int array[], int size)
{
    int elimina = 0, tmp;
    printf("\nDigita numero da eliminare: ");
    scanf("%d", &elimina);

    for (int i = 0; i <= size; i++)
    {
        if (array[i] == elimina)
        {
            tmp = array[i];
            array[i] = array[size - 1];
            array[size - 1] = tmp;
            printf("Numero eliminato\n");
            break;
        }
    }
    return size + 1;
}

int Ordina(int array[], int size)
{
    for (int i = 0; i <= size; i++)
    {
        for (int j = (i + 1); j < size; j++)
        {
            if (array[i] > array[j])
            {
                int tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }
    }
    return size + 1;
}

void Stampa(int array[], int size)
{
    for (int i = 0; i <= size; i++)
        printf("| %d | ", array[i]);
}

int main()
{
    int *array = malloc(sizeof(int));
    int size = 0, scelta = 0, esci = 0;

    do
    {
        printf("\nScegli:\
        \n'1' -> Inserisci\
        \n'2' -> Verifica\
        \n'3' -> Elimina\
        \n'4' -> Ordina\
        \n'0' -> Esci\
        \n-> ");
        scanf("%d", &scelta);

        switch (scelta)
        {
        case 0:
            esci = 1;
            break;
        case 1:
            Inserisci(array, size);
            Stampa(array, size);
            break;
        case 2:
            Verifica(array, size);
            Stampa(array, size);
            break;
        case 3:
            Elimina(array, size);
            Stampa(array, size);
            break;
        case 4:
            Ordina(array, size);
            Stampa(array, size);
            break;
        }
    } while (esci == 0);

    return 0;
}