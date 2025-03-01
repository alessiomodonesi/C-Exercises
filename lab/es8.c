// Alessio Modonesi 5F

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE 100

void InitArray(char *arr)
{
    for (int i = 0; i < SIZE + 1; i++)
        arr[i] = '\0';
}

void Sort(char *str)
{
    int k = 0, x = 0, z = 0;
    char letter[SIZE + 1], number[SIZE + 1], special[SIZE + 1];

    InitArray(letter);
    InitArray(number);
    InitArray(special);

    for (int i = 0; i < strlen(str); i++)
    {
        if (isalpha(str[i]) == 1) // maiuscola
        {
            for (int j = (i + 1); j < strlen(str); j++)
            {
                if (str[i] > str[j] && isalpha(str[j]) != 0)
                {
                    int c = str[i];
                    str[i] = str[j];
                    str[j] = c;
                }
            }
            letter[k++] = str[i];
        }
        else if (isalpha(str[i]) == 2) // minuscola
        {
            for (int j = (i + 1); j < strlen(str); j++)
            {
                if (str[i] > str[j] && isalpha(str[j]) == 0)
                {
                    int c = str[i];
                    str[i] = str[j];
                    str[j] = c;
                }
            }
            letter[k++] = str[i];
        }
        else if (isdigit(str[i]) == 1) // numero
            number[x++] = str[i];
        else if (isalpha(str[i]) == 0) // speciale
            special[z++] = str[i];
    }

    char finalString[SIZE + 1];
    sprintf(finalString, "%s%s%s", letter, number, special);
    finalString[SIZE] = '\0';

    printf("Final string: %s\n", finalString);
}

int main()
{
    char string[SIZE];
    while (1)
    {
        printf("Insert a string or 'esc': ");
        scanf("%s", string);
        if (strcmp(string, "esc") != 0)
            Sort(string);
        else
            return 0;
    }
}