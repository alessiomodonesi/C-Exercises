// Alessio Modonesi 5F
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10
#define NDEL 0
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
            int tmp = arr[i];
            arr[i] = arr[size - 1];
            arr[size - 1] = tmp;
            break;
        }
    }
}

void BuildPari(int *arr, int dim)
{
    int num = 0, p = 0;
    for (int i = 0; i < dim; i++)
    {
        if (arr[i] % 2 == 0)
            num++;
    }

    int *pari = malloc(sizeof(int) * num); // sizeof(pari) SEMPRE = 8 byte
    for (int i = 0; i < dim; i++)
    {
        if (arr[i] % 2 == 0)
        {
            pari[p++] = arr[i];
        }
    }
    Sort(pari, num, 1);
    printf("\n\nPari\n");
    for (int i = 0; i < num; i++)
        printf("[%d]: %d\n", i, pari[i]);
}

void BuildDispari(int *arr, int dim)
{
    int num = 0, d = 0;
    for (int i = 0; i < dim; i++)
    {
        if (arr[i] % 2 != 0)
            num++;
    }

    int *dispari = malloc(sizeof(int) * num); // sizeof(pari) SEMPRE = 8 byte
    for (int i = 0; i < dim; i++)
    {
        if (arr[i] % 2 != 0)
        {
            dispari[d++] = arr[i];
        }
    }
    Sort(dispari, num, 0);
    printf("\nDispari\n");
    for (int i = 0; i < num; i++)
        printf("[%d]: %d\n", i, dispari[i]);
}

int main()
{
    srand(time(NULL));
    int size1 = SIZE, size2 = NDEL;
    int *array = malloc(sizeof(int) * size1);
    int *delete = malloc(sizeof(int) * size2);
    int d = 0, fr = 0;

    Generation(array);
    Sort(array, size1, 0);
    Print(array, size1);

    for (int i = 0; i < size1; i++)
    {
        fr = Count(array, i);
        if (fr > 1)
        {
            printf("\nnum: %d - rep: %d", array[i], fr);

            if (fr > 2)
                size2 += fr - 2;
            delete = realloc(delete, sizeof(int) * ++size2);

            int cont = 0;
            while (cont <= (fr - 2))
            {
                delete[d++] = array[i];
                cont++;
            }
            i += fr - 1;
        }
    }

    for (int i = 0; i < size2; i++)
    {
        Delete(array, delete[i], size1);
        size1--;
    }

    BuildPari(array, size1);
    BuildDispari(array, size1);
}