// Alessio Modonesi 5F
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10
#define MAX 20
#define MIN 3
#define MIN_A 0
#define MAX_Z 26

int main()
{
    char array[SIZE][MAX];
    char alpha[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    int dim, lettera;
    // FILE *fp = fopen("file.txt", "w+");

    for (int i = 0; i < SIZE; i++) // itera SIZE volte per riempire tutto l'array
    {
        dim = (rand() % (MAX - MIN + 1)) + MIN; // genero random la dimensione della stringa
        // printf("dim: %d\n", dim);

        for (int j = 0; j < dim; j++) // itera dim volte, ovvero la dimensione della stringa
        {
            lettera = (rand() % (MAX_Z - MIN_A + 1)) + MIN_A; // genero random il char della stringa
            /*printf("%c", alpha[lettera]);
            if (j == (dim - 1))
                printf("%c: ", alpha[lettera]);*/
            array[i][j] = alpha[lettera];
            // printf("%c ", array[i][j]);
        }

        if (strcmp(array[i], array[i - 1]) == 0)
            printf("DOPPIONE\n");
        else
        {
            for (int j = 0; j < dim; j++)
            {
                if (j != (dim - 1))
                {
                    printf("%c", array[i][j]);
                    // fprintf(fp, "%c", array[i][j]);
                }
                else
                {
                    printf("%c\n", array[i][j]);
                    // fprintf(fp, "%c\n", array[i][j]);
                }
            }
        }
    }
    // fclose(fp);
    return 0;
}