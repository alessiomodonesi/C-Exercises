// Alessio Modonesi 5F

#include <stdio.h>
#include <stdlib.h>

int Inserisci(int array[], int size)
{
    array = realloc(array, (size + 1) * sizeof(int));

    printf("\nInserisci un numero: ");
    scanf("%d", &array[size + 1]);

    return (size + 1);
}

int Verifica(int array[], int size)
{
    int trova = 0;
    printf("\nDigita numero da cercare: ");
    scanf("%d", &trova);

    for (int i = 1; i <= size; i++)
    {
        if (array[i] == trova)
            return (i - 1);
    }

    return -1;
}

int Elimina(int array[], int size)
{
    int elimina, var = 0;
    printf("\nDigita numero da eliminare: ");
    scanf("%d", &elimina);

    for (int i = 1; i <= size; i++)
    {
        if (elimina != array[i])
        {
            array[var++] = array[i];
        }
    }

    return (var - 1);
}

void Ordina(int array[], int size)
{
    for (int i = 1; i <= size; i++)
    {
        for (int j = (i + 1); j <= size; j++)
        {
            if (array[i] > array[j])
            {
                int tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }
    }
    printf("\n");
}

void Stampa(int array[], int size)
{
    printf("array: ");
    for (int i = 1; i <= size; i++)
        printf("%d ", array[i]);
    printf("\n\n");
}

int main()
{
    int *array = malloc(sizeof(int));
    int size = 0, pos = 0, scelta = 0;

    do
    {
        printf("Scegli:\
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
            break;
        case 1:
            size = Inserisci(array, size);
            Stampa(array, size);
            break;
        case 2:
            pos = Verifica(array, size);
            printf("Numero trovato in posizione: %d\n", pos);
            Stampa(array, size);
            break;
        case 3:
            size = Elimina(array, size);
            Stampa(array, size);
            break;
        case 4:
            Ordina(array, size);
            Stampa(array, size);
            break;
        }
    } while (scelta != 0);

    free(array);
}