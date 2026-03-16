// Alessio Modonesi 5F
// Esercizio 4 – Gestione Stringhe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define SIZE 100

void Funzione123(char *str, char *array) // funzionante
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
    printf("\n- %d consonanti", (cons / 9) - 1);
    printf("\n- %lu numeri", (num / strlen(array)));
    printf("\n- %lu non-alphabetic\n", (other / strlen(array)));
}

void Funzione4(char *str, char *cerca) // funzionante
{
    int cont = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == cerca[0])
            cont++;
    }

    printf("\nLa stringa '%s' contiene %d volte il char %s\n", str, cont, cerca);
}

void Funzione5(char *str) // funzionante
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
    printf("\nLa stringa dispari è: %s\n", dispari);
}

void Funzione6(char *str) // funzionante
{
    int doppie = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = (i + 1); j < strlen(str); j++)
        {
            if (str[i] == str[j])
                doppie = 1;
        }
    }

    if (doppie == 1)
        printf("\nTrovate lettere doppie\n");
    else
        printf("\nNon ci sono lettere doppie\n");
}

void Funzione7(char *str, char *str2) // funzionante
{
    if (strlen(str) > strlen(str2))
        printf("\n'%s' è più lunga\n", str);
    else if (strlen(str) < strlen(str2))
        printf("\n'%s' è più lunga\n", str2);
    else
        printf("\n'%s' e '%s' sono lunghe uguali\n", str, str2);
}

void Funzione8(char *str, char *str2) // funzionante
{
    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = 0; j < strlen(str2); j++)
        {
            if (str[i] == str2[j])
                printf("\n'%s' e '%s' contengono entrambe '%c'", str, str2, str[i]);
        }
    }
    printf("\n");
}

void Funzione910(char *str, char *str2, char *array) // funzionante
{
    int voc1 = 0, voc2 = 0, cons1 = 0, cons2 = 0, num1 = 0, num2 = 0, other1 = 0, other2 = 0;

    for (int i = 0; i < strlen(str); i++)
    {
        char c = toupper(str[i]); // trasformo il char in MAIUSCOLO
        for (int j = 0; j < strlen(array); j++)
        {
            if (c == array[j]) // c è una vocale
                voc1++;
            else if (isdigit(c) == 1) // c è un numero
                num1++;
            else if (isalpha(c) == 0) // c è non-alpha numerico
                other1++;
            else // per arrivare qui è per forza una consonante
                cons1++;
        }
    }

    for (int i = 0; i < strlen(str2); i++)
    {
        char c = toupper(str2[i]); // trasformo il char in MAIUSCOLO
        for (int j = 0; j < strlen(array); j++)
        {
            if (c == array[j]) // c è una vocale
                voc2++;
            else if (isdigit(c) == 1) // c è un numero
                num2++;
            else if (isalpha(c) == 0) // c è non-alpha numerico
                other2++;
            else // per arrivare qui è per forza una consonante
                cons2++;
        }
    }

    if (voc1 > voc2)
        printf("\n'%s' ha più vocali", str);
    else if (voc1 < voc2)
        printf("\n'%s' ha più vocali", str2);
    else
        printf("\n'%s' e '%s' hanno le stesse vocali", str, str2);

    if (cons1 > cons2)
        printf("\n'%s' ha più consonanti\n", str);
    else if (cons1 < cons2)
        printf("\n'%s' ha più consonanti\n", str2);
    else
        printf("\n'%s' e '%s' hanno le stesse consonanti\n", str, str2);
}

int main()
{
    char cerca[1];
    int scelta, esci = 0;
    char voc[5] = {'A', 'E', 'I', 'O', 'U'};

    char string[SIZE];
    printf("\nInserisci la 1a stringa: ");
    scanf("%s", &string);

    char string2[SIZE];
    printf("\nInserisci la 2a stringa: ");
    scanf("%s", &string2);

    do
    {
        printf("\nScegli un'azione:\
        \n'1' per analizzare str1\
        \n'2' per cercare un char\
        \n'3' per creare 2 str pari e dispari\
        \n'4' per verificare le doppie\
        \n'5' per confrontare str1 & str2\
        \n'6' per vedere le lettere in comune\
        \n'7' per analizzare str1 & str2\
        \n'0' per uscire\
        \n--> ");
        scanf("%d", &scelta);

        switch (scelta)
        {
        case 0:
            esci = 1;
            break;
        case 1:
            Funzione123(string, voc);
            break;
        case 2:
            printf("\nDigita char da cercare: ");
            scanf("%s", &cerca);
            Funzione4(string, cerca);
            break;
        case 3:
            Funzione5(string);
            break;
        case 4:
            Funzione6(string);
            break;
        case 5:
            Funzione7(string, string2);
            break;
        case 6:
            Funzione8(string, string2);
            break;
        case 7:
            Funzione910(string, string2, voc);
            break;
        }
    } while (esci == 0);

    return 0;
}