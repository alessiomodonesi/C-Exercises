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

#define SIZE 10

int *Inserisci(int array[])
{
    int num;
    for (int i = 0; i < SIZE; i++)
    {
        printf("Inserisci un numero: ");
        scanf("%d", &num);
        array[i] = num;
    }
    return array;
}

int *Verifica(int array[])
{
    if (sizeof(array) != SIZE)
    {
    }
    return array;
}

int *Elimina(int array[])
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
    return array;
}

int *Ordina(int array[])
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
    return array;
}

void Stampa(int array[])
{
    for (int i = 0; i < SIZE; i++)
        printf("| %d | ", array[i]);
}

int main()
{
    int array[SIZE];
    int scelta = 0, esci = 0;

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
            Inserisci(array);
            Stampa(array);
            break;
        case 2:
            Verifica(array);
            Stampa(array);
            break;
        case 3:
            Elimina(array);
            Stampa(array);
            break;
        case 4:
            Ordina(array);
            Stampa(array);
            break;
        }
    } while (esci == 0);

    return 0;
}