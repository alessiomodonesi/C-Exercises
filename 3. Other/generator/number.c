#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

int printRandoms(int lower, int upper, int count)
{
    int i;
    int num;
    for (i = 0; i < count; i++)
    {
        num = (rand() %
               (upper - lower + 1)) +
              lower;
    }
    return num;
}

#define NWORDS 10000

int Comparewords(char *ArrayString[], int nelements)
{
    int result;
    result = 1;
    for (int jj = 0; jj < nelements - 1; jj++)
    {
        if (strcmp(ArrayString[jj], ArrayString[nelements - 1]) == 0)
        {
            result = 2;

            printf("TROVATE DUE PAROLE UGUALI\n");
            printf("Le parole erano: %s\n", ArrayString[jj]);
            break;
        }
    }
    return result;
}

int main()
{

    char char1[36] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}; // stringa costante da cui vado a prendere i caratteri scrivibili

    char *ArrayString[NWORDS];
    int index = 0;
    FILE *fp;
    fp = fopen("file.txt", "w+");
    // contatori
    int wordlength;
    int idx;
    int ii;
    int result;

    for (int i = 0; i < NWORDS; i++)
    {
        wordlength = printRandoms(3, 20, 1);

        ArrayString[i] = (char *)malloc((wordlength + 1) * sizeof(char));

        for (index = 0; index < wordlength; index++)
        {
            idx = printRandoms(0, 35, 1);
            ArrayString[i][index] = char1[idx];
        }
        ArrayString[i][wordlength] = '\0';

        if (i >= 1)
            result = Comparewords(ArrayString, i + 1);

        if (result == 2)
            i--;
    }

    printf("Le parole generate sono:");
    for (ii = 0; ii < NWORDS; ii++)
        fprintf(fp, " %s\n", ArrayString[ii]);
    fclose(fp);

    for (ii = 0; ii < NWORDS; ii++)
        free(ArrayString[ii]);

    return 0;
}
