// Alessio Modonesi 5F

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE 100

void Sort(char *string)
{
    int j = 0, k = 0;
    char str[SIZE];
    char num[SIZE];
    char sc[SIZE];

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
                    str[i] = c;
                }
            }
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
                    str[i] = c;
                }
            }
        }
        else if (isdigit(string[i]) == 1) // numero
        {
            num[j] = string[i];
            j++;
        }
        else if (isalpha(string[i]) == 0) // carattere speciale
        {
            sc[k] = string[i];
            k++;
        }
    }

    if (j > 0)
        string = strcat(str, num);
    if (k > 0)
        string = strcat(str, sc);
}

int main()
{
    char string[SIZE];

    printf("\nInserisci la tua string: ");
    scanf("%s", string);

    Sort(string);
    printf("\nSorted string: %s", string);
    printf("\n\n");
}