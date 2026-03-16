// Alessio Modonesi 5F
#include <stdio.h>
#include <stdlib.h>

#define SIZE 5
#define TEST 0

int *SetDati(int array[])
{
    for (int i = 0; i < SIZE; i++)
    {
        printf("\nInserisci un int: ");
        scanf("%d", &array[i]);
    }
    return array;
}

int Check(int array[])
{
    for (int i = 0; i < SIZE; i++)
    {
        if (array[i] == TEST)
            return 1;
    }
    return 0;
}

int main()
{
    int array[SIZE];
    SetDati(array);

    if (Check(array) == 1)
        printf("\nErrore");
    else
        printf("\nTutto a buon fine");

    return 0;
}