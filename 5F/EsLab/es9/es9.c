#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100
#define MIN 48
#define MAX 57

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

int Search(char *str, int flag)
{
    int dim = 0;
    if (flag == 0) // cerca numeri
    {
        for (int i = 0; i < strlen(str); i++)
        {
            if (str[i] >= MIN && str[i] <= MAX) // cerco i numeri
                dim++;
        }
    }
    else if (flag == 1) // cerca pari
    {
        int num = 0;
        for (int i = 0; i < strlen(str); i++)
        {
            num = (str[i] - MIN);
            if (num % 2 == 0) // cerco i pari
                dim++;
        }
    }
    else // cerca dispari
    {
        int num = 0;
        for (int i = 0; i < strlen(str); i++)
        {
            num = (str[i] - MIN); // cerco i dispari
            if (num % 2 != 0)
                dim++;
        }
    }
    return dim;
}

char *BuildArray(char *str)
{
    int dim = Search(str, 0);
    char *arr = malloc(dim + 1);

    int k = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] >= MIN && str[i] <= MAX) // cerco i numeri
        {
            arr[k] = str[i];
            k++;
        }
    }
    arr[k] = '\0';
    return arr;
}

void BuildPari(char *str)
{
    int dim = Search(str, 1);
    int *pari = malloc(dim + 1);

    int num = 0, k = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        num = (str[i] - MIN);
        if (num % 2 == 0)
        {
            pari[k] = num;
            k++;
        }
    }
    Sort(pari, k, 0);

    printf("\nPari\n");
    for (int i = 0; i < dim; i++)
        printf("[%d]: %d\n", i, pari[i]);
}

void BuildDispari(char *str)
{
    int dim = Search(str, 1);
    int *dispari = malloc(dim + 1);

    int num = 0, k = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        num = (str[i] - MIN);
        if (num % 2 != 0)
        {
            dispari[k] = num;
            k++;
        }
    }
    Sort(dispari, k, 1);

    printf("\nDispari\n");
    for (int i = 0; i < dim; i++)
        printf("[%d]: %d\n", i, dispari[i]);
}

int main()
{
    char string[SIZE];
    while (1)
    {
        printf("Insert a string or 'esc': ");
        scanf("%s", string);
        if (strcmp(string, "esc") != 0)
        {
            char *array = BuildArray(string);
            BuildPari(array);
            BuildDispari(array);
            printf("\n");
        }
        else
            return 0;
    }
}