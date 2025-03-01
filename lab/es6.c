// Alessio Modonesi 5F
#include <stdio.h>
#include <stdlib.h>

#define SIZE 100
#define PARI 20
#define DISPARI 20
#define ALTRI 60

void StampaArray(int array[], int size)
{
    for (int i = 0; i < size; i++)
        printf("[%d]: %d\n", i, array[i]);
    printf("\n\n");
}

void Genera(int array[])
{
    int i = 0, j = 0;
    while (i < SIZE)
    {
        int num = rand() % 233 + 1;

        for (j = 0; j < i; j++)
        {
            if (array[j] == num)
                break;
        }

        if (j == i)
            array[i++] = num;
    }
}

int Check(int array[])
{
    int pari = 0;
    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] % 2 == 0)
            pari++;
    }
    return pari;
}

void Ordina(int array[], int size, int reverse)
{
    if (reverse == 0)
    {
        for (int i = 0; i < size; i++)
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
    }
    else
    {
        for (int i = 0; i < size; ++i)
        {
            for (int j = i + 1; j < size; ++j)
            {
                if (array[i] < array[j])
                {
                    int tmp = array[i];
                    array[i] = array[j];
                    array[j] = tmp;
                }
            }
        }
    }
}

void Move(int array[], int arrayP[], int arrayD[], int arrayA[])
{
    int pari = 0, dispari = 0, altri = 0;
    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] % 2 == 0 && pari < 20)
        {
            arrayP[pari] = array[i];
            pari++;
        }
        else if (array[i] % 2 != 0 && dispari < 20)
        {
            arrayD[dispari] = array[i];
            dispari++;
        }
        else
        {
            arrayA[altri] = array[i];
            altri++;
        }
    }
}

int main()
{
    int array[SIZE];
    int arrayP[PARI];
    int arrayD[DISPARI];
    int arrayA[ALTRI];
    int pari, dispari;

    do
    {
        Genera(array);
        pari = Check(array);
        dispari = 100 - pari;
    } while (pari < 20 && dispari < 20);

    Ordina(array, SIZE, 0);
    Move(array, arrayP, arrayD, arrayA);
    Ordina(arrayD, DISPARI, 1);

    // StampaArray(array, SIZE);
    StampaArray(arrayP, PARI);
    StampaArray(arrayD, DISPARI);
    StampaArray(arrayA, ALTRI);
}