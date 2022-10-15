/*FILA B:
1) Le frequenze dei numeri inseriti e/o generati;
2) Elimini i numeri che si ripetono più di una volta all’interno del vettore, poi aggiustando la dimensione del vettore affinché risulti pieno;
3) Ordini, partendo dal vettore iniziale, i numeri dispari in ordine crescente e i pari in ordine
decrescente.*/

// Alessio Modonesi 5F
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10
#define MIN 0
#define MAX 9
#define NUM(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))

void Generation(int *arr)
{
    for (int i = 0; i < SIZE; i++)
    {
        int num = NUM(MIN, MAX);
        arr[i] = num;
    }
}

void Print(int *arr, int dim)
{
    for (int i = 0; i < dim; i++)
        printf("[%d]: %d\n", i, arr[i]);
}

void Sort(int *arr, int size, int flag)
{
    if (flag == 0)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = (i + 1); j < size; j++)
            {
                if (arr[i] > arr[j])
                {
                    int tmp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = tmp;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < size; ++i)
        {
            for (int j = (i + 1); j < size; ++j)
            {
                if (arr[i] < arr[j])
                {
                    int tmp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = tmp;
                }
            }
        }
    }
}

int Count(int *arr, int i)
{
    int cont = 1;
    for (int j = i + 1; j < SIZE; j++)
    {
        if (arr[i] == arr[j])
            cont++;
    }
    return cont;
}

int Delete(int *arr, int delete)
{
    int nDelete = 0;
    for (int i = 0; i < SIZE; i++)
    {
        if (arr[i] == delete)
        {
            nDelete++;
            do
            {
                arr[i] = NUM(MIN, MAX);
            } while (arr[i] != delete);
        }
    }
    return nDelete;
}

/*void BuildPari(int *arr, int dim)
{
    int pari[dim], num = 0, k = 0;
    for (int i = 0; i < dim; i++)
    {
        if (arr[i] % 2 == 0)
        {
            pari[k] = num;
            k++;
        }
    }
    Sort(pari, k, 1);

    printf("\nPari\n");
    for (int i = 0; i < dim; i++)
        printf("[%d]: %d\n", i, pari[i]);
}*/

/*void BuildDispari(int *arr, int dim)
{
    int dispari[dim], num = 0, k = 0;
    for (int i = 0; i < dim; i++)
    {
        if (arr[i] % 2 == 0)
        {
            dispari[k] = num;
            k++;
        }
    }
    Sort(dispari, k, 0);

    printf("\nDispari\n");
    for (int i = 0; i < dim; i++)
        printf("[%d]: %d\n", i, dispari[i]);
}*/

int main()
{
    int array[SIZE];
    int nDel = 0, fr = 0;
    srand(time(NULL));

    Generation(array);
    Sort(array, sizeof(array) / sizeof(int), 0);
    Print(array, sizeof(array) / sizeof(int));

    for (int i = 0; i < SIZE; i++)
    {
        fr = Count(array, i);
        if (fr > 1)
        {
            printf("\nIl numero '%d' compare '%d' volte", array[i], fr);
            nDel = Delete(array, array[i]);
        }
    }
    printf("\n\n");
    Print(array, (sizeof(array) / sizeof(int)) - nDel);

    // BuildPari(array, (sizeof(array) / sizeof(int)) - nDel);
    // BuildDispari(array, (sizeof(array) / sizeof(int)) - nDel);
}
