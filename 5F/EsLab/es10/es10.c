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

void Delete(int *arr, int canc, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == canc)
        {
            printf("Sto eliminando il numero: %d\n", arr[i]);
            int tmp = arr[i];
            arr[i] = arr[size - 1];
            arr[size - 1] = tmp;
        }
    }
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
    int fr = 0, size = SIZE;
    int array[size];
    int delete[SIZE];
    srand(time(NULL));

    Generation(array);
    Sort(array, size, 0);
    Print(array, size);

    for (int i = 0; i < size; i++)
    {
        fr = Count(array, i);
        if (fr > 1)
        {
            printf("\nIl numero '%d' compare '%d' volte", array[i], fr);
            delete[i] = array[i];
            i += fr - 1;
        }
    }

    printf("\nDelete\n");
    for (int i = 0; i < SIZE; i++)
    {
        printf("%d", delete[i]);
        // Delete(array, delete[i], size);
        // size--;
    }
    // Sort(array, size, 0);
    // printf("\n\n");
    // Print(array, size);

    // BuildPari(array, (sizeof(array) / sizeof(int)) - nDel);
    // BuildDispari(array, (sizeof(array) / sizeof(int)) - nDel);
}
