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
    scanf("%[^\n]%*c", str); // legge fino a quando non trova \n
    printf("Calcolo in corso di '%s'\n", str);

    for (int i = 0; i < strlen(str); i++)
        somma += str[i];

    printf("\nascii: %d", somma);
    if (somma >= 2000)
        printf("\nascii >= 2000");
    else
        printf("\nascii < 2000");
    return 0;
}