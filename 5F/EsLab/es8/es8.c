// Alessio Modonesi 5F

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE 100

void Sort(char *string)
{
    int s = 0;
    char str[SIZE + 1];
    int n = 0;
    char num[SIZE + 1];
    int p = 0;
    char special[SIZE + 1];

    for (int i = 0; i < strlen(string); i++)
    {
        if (isalpha(string[i]) == 1) // lettera maiuscola
        {
            for (int j = i + 1; j < strlen(string); j++)
            {
                if (string[i] > string[j] && isalpha(string[j]) != 0)
                {
                    int c = string[i];
                    string[i] = string[j];
                    string[j] = c;
                }
            }
            str[s++] = string[i];
        }
        else if (isalpha(string[i]) == 2) // lettera minuscola
        {
            for (int j = i + 1; j < strlen(string); j++)
            {
                if (string[i] > string[j] && isalpha(string[j]) == 0)
                {
                    int c = string[i];
                    string[i] = string[j];
                    string[j] = c;
                }
            }
            str[s++] = string[i];
        }
        else if (isdigit(string[i]) == 1) // numero
        {
            num[n++] = string[i];
        }
        else if (isalpha(string[i]) == 0) // carattere speciale
        {
            special[p++] = string[i];
        }
        else
            printf("Char non riconosciuto");
    }

    int size = strlen(str);
    str[size] = '\0';

    size = strlen(num);
    num[size] = '\0';

    size = strlen(special);
    special[size] = '\0';

    for (int i = 0; i < strlen(str); i++)
        printf("%c", str[i]);

    if (n > 0 && p > 0)
    {
        for (int i = 0; i < strlen(num); i++)
            putchar(num[i]);

        for (int j = 0; j < strlen(special); j++)
            putchar(special[j]);
    }
    else if (n > 0)
    {
        for (int i = 0; i < strlen(num); i++)
            putchar(num[i]);
    }
    else if (p > 0)
    {
        for (int i = 0; i < strlen(special); i++)
            putchar(special[i]);
    }

    printf("\n");
}

int main()
{
    char string[SIZE];
    while (1)
    {
        printf("Inserisci una stringa o 'esci' per uscire: ");
        scanf("%s", string);

        if (strcmp(string, "esci") != 0)
            Sort(string);
        else
            return 0;
    }
}