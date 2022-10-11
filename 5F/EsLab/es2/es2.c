// Alessio Modonesi 5F
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100

int main()
{
    char str[SIZE];
    int somma = 0;

    printf("\nInserisci una parola: ");
    scanf("%s", str);

    for (int i = 0; i < strlen(str); i++)
        somma += str[i];

    printf("\nTotale: %d", somma);

    if (somma >= 2000)
        printf("\nAscii value >= 2000");
    else
        printf("\nAscii value < 2000");

    return 0;
}