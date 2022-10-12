// Alessio Modonesi 5F

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE 100

void PrintChar(char *string)
{
    int num = 0, s = 0;

    printf("\nLa tua stringa contiene:");
    for (int i = 0; i < strlen(string); i++)
    {
        char c = toupper(string[i]);

        if (i == 0)
        {
            num++;
            printf("\nchar '%c', %d ripetizione/i", c, num);
        }

        if (string[i] == string[++s] && i > 0)
        {
            num++;
            printf("\nchar '%c', %d ripetizione/i", c, num);
        }
        else if (i > 0)
            printf("\nchar '%c', %d ripetizione/i", c, num);
    }
}

int main()
{
    char string[SIZE];

    printf("\nInserisci la tua stringa: ");
    scanf("%s", string);

    PrintChar(string);
    printf("\n\n");
}