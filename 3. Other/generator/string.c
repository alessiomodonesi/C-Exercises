// Alessio Modonesi 5F

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    char stringa[50];
    for (int i = 0; i < 50; i++)
    {
        char c = 32 + rand() % 95;
        if (strchr(stringa, c) == NULL || i == 0)
            stringa[i] = c;
        else
            i--;
    }
    printf("%s", stringa);
}