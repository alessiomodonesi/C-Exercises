// Alessio Modonesi 5F
// Esercizio 4 – Gestione Stringhe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 100

void Funzione123(char *str, char *array)
{
    int voc = 0, cons = 0, num = 0, other = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        char c = toupper(str[i]); // trasformo il char in MAIUSCOLO
        for (int j = 0; j < strlen(array); j++)
        {
            if (c == array[j]) // c è una vocale
                voc++;
            else if (isdigit(c) == 1) // c è un numero
                num++;
            else if (isalpha(c) == 0) // c è non-alpha numerico
                other++;
            else // per arrivare qui è per forza una consonante
                cons++;
        }
    }

    printf("\nLa stringa '%s' contiene:", str);
    printf("\n- %d vocali", voc);
    printf("\n- %d consonanti", (cons / 9));                          // divido per 9 ???
    printf("\n- %lu numeri", (num / strlen(array)));                  // divido per sizeof(array)
    printf("\n- %lu non-alphabetic char\n", (other / strlen(array))); // divido per sizeof(array)
}

void Funzione4(char *str, char cerca)
{
    int cont = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (strcmp(&str[i], &cerca) == 1)
            cont++;
    }

    printf("\nLa stringa '%s' contiene %d volte il char %c\n", str, cont, cerca);
}

void Funzione5(char *str)
{
    char pari[100], dispari[100];

    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = 0; j < strlen(pari); j++)
        {
            printf("i: %d\n", i);
            printf("j: %d\n", j);

            if ((i % 2) == 0)
                pari[j] += str[i];
            else
                dispari[j] += str[i];
        }
    }

    printf("\nLa stringa pari è: %s", pari);
    printf("\nLa stringa dispari è: %s", dispari);
}

void Funzione6(char *str)
{
}

void Funzione7(char *str)
{
}

void Funzione8(char *str)
{
}

void Funzione9(char *str)
{
}

void Funzione10(char *str)
{
}

int main()
{

    char string[SIZE], cerca;
    char voc[5] = {'A', 'E', 'I', 'O', 'U'};

    printf("Inserisci la 1a stringa: ");
    scanf("%[^\n]", string);
    Funzione123(string, voc);

    printf("\nDigita char da cercare: ");
    scanf("%s", &cerca);
    Funzione4(string, cerca);

    Funzione5(string);

    Funzione6(string);

    // printf("Inserisci la 2a stringa: ");
    // scanf("%s", string);

    Funzione7(string);
    Funzione8(string);
    Funzione9(string);
    Funzione10(string);

    return 0;
}