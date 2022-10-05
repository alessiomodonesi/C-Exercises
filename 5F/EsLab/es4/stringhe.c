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

    printf("La stringa '%s' contiene %d volte il char %c\n", str, cont, cerca);
}

void Funzione5(char *str)
{
    int size, sizeP, sizeD, j = 0;
    if ((sizeof(str)) % 2 == 0)
    {
        size = strlen(str) / 2;
        sizeP = size;
        sizeD = size;
    }
    else
    {
        size = strlen(str) + 1;
        sizeD = (size / 2) + 1;
        sizeP = (size / 2);
    }

    char pari[sizeP], dispari[sizeD];

    // printf("\nsize P: %d", sizeP);
    // printf("\nsize D: %d", sizeD);

    pari[sizeP - 1] = '\0';
    dispari[sizeD - 1] = '\0';

    for (int i = 0; i < strlen(str); i++)
    {
        if ((i % 2) == 0)
        {
            pari[j] = str[i];
            j++;
        }
    }

    j = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if ((i % 2) != 0)
        {
            dispari[j] = str[i];
            j++;
        }
    }

    printf("\nLa stringa pari è: %s", pari);
    printf("\nLa stringa dispari è: %s", dispari);
}

void Funzione6(char *str)
{
    int cont = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = (i + 1); j < strlen(str); j++)
        {
            if (str[i] == str[j])
                cont++;
        }
    }

    printf("\n\nTrovate %d lettere doppie", cont);
}

void Funzione7(char *str, char *str2)
{
    if (strlen(str) > strlen(str2))
        printf("\n'%s' è più lunga", str);
    else if (strlen(str) < strlen(str2))
        printf("\n'%s' è più lunga", str2);
    else
        printf("\n'%s' e '%s' sono lunghe uguali", str, str2);
}

void Funzione8(char *str, char *str2)
{
    char ambedue;
    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = 0; j < strlen(str2); j++)
        {
            if (str[i] == str2[j])
                printf("\n'%s' e '%s' contengono entrambe '%c'", str, str2, ambedue);
        }
    }
}

void Funzione910(char *str, char *str2, char *array)
{
    int voc1 = 0, voc2 = 0, cons1 = 0, cons2 =, num1 = 0, num2 = 0, other1 = 0, other2 = 0;
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

    printf("\nLa stringa '%s' contiene:", str2);
    printf("\n- %d vocali", voc);
    printf("\n- %d consonanti", (cons / 9));                          // divido per 9 ???
    printf("\n- %lu numeri", (num / strlen(array)));                  // divido per sizeof(array)
    printf("\n- %lu non-alphabetic char\n", (other / strlen(array))); // divido per sizeof(array)
}

int main()
{
    char voc[5] = {'A', 'E', 'I', 'O', 'U'};

    char string[SIZE], cerca;
    printf("Inserisci la 1a stringa: ");
    scanf("%[^\n]", string);
    Funzione123(string, voc);
    printf("\nDigita char da cercare: ");
    scanf("%s", &cerca);
    Funzione4(string, cerca);
    Funzione5(string);
    Funzione6(string);

    char string2[SIZE];
    printf("\nInserisci la 2a stringa: ");
    scanf("%[^\n]", string2);
    Funzione7(string, string2);
    Funzione8(string, string2);
    Funzione9(string, string2, array);
    Funzione10(string, string2, array);

    return 0;
}