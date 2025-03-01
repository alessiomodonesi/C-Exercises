#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int sockfd;
    int connfd;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    char risposta[1024];
    int i, j;
    int cont;
    int contVocali;
    int contConsonanti;

    // Creazione del socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Errore nell'apertura del socket\n");
        return 1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(1234);

    // Binding del socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Errore nel binding\n");
        return 1;
    }

    // Listening con massimo 5 connessioni in coda
    if (listen(sockfd, 5) < 0)
    {
        printf("Errore nell'ascolto\n");
        return 1;
    }

    // Accetta la connessione in arrivo
    int len = sizeof(serv_addr);
    connfd = accept(sockfd, (struct sockaddr *)&serv_addr, &len);
    if (connfd < 0)
    {
        printf("Errore nell'accettazione\n");
        return 1;
    }

    // Ricezione del numero dell'esercizio da eseguire
    int num;
    int n = recv(connfd, buffer, 1024, 0);
    buffer[n] = '\0';
    num = buffer[0];

    // Ricezione dei dati necessari
    switch (num)
    {
    case 1:
    {
        // Ricezione della stringa
        n = recv(connfd, buffer, 1024, 0);
        buffer[n] = '\0';
        char str[50];
        strcpy(str, buffer);

        // Controllo palindromo
        int palindromo = 1;
        for (i = 0, j = strlen(str) - 1; i < j; i++, j--)
        {
            if (str[i] != str[j])
            {
                palindromo = 0;
                break;
            }
        }

        // Risposta al client
        if (palindromo)
            strcpy(risposta, "La stringa e' palindroma");
        else
            strcpy(risposta, "La stringa non e' palindroma");
        break;
    }
    case 2:
    {
        // Ricezione della stringa
        n = recv(connfd, buffer, 1024, 0);
        buffer[n] = '\0';
        char str[50];
        strcpy(str, buffer);
        char c = str[strlen(str) - 1];
        str[strlen(str) - 1] = '\0';

        // Ricerca del carattere
        cont = 0;
        for (i = 0; i < strlen(str); i++)
        {
            if (str[i] == c)
                cont++;
        }

        // Risposta al client
        sprintf(risposta, "Il carattere e' presente %d volte", cont);
        break;
    }
    case 3:
    {
        // Ricezione della stringa
        n = recv(connfd, buffer, 1024, 0);
        buffer[n] = '\0';
        char str[50];
        strcpy(str, buffer);

        // Conteggio vocali e consonanti
        contVocali = 0;
        contConsonanti = 0;
        for (i = 0; i < strlen(str); i++)
        {
            if (str[i] == 'a' || str[i] == 'e' || str[i] == 'i' || str[i] == 'o' || str[i] == 'u')
                contVocali++;
            else if (str[i] >= 'a' && str[i] <= 'z')
                contConsonanti++;
        }

        // Risposta al client
        sprintf(risposta, "Ci sono %d vocali e %d consonanti", contVocali, contConsonanti);
        break;
    }
    case 4:
    {
        // Ricezione della stringa
        n = recv(connfd, buffer, 1024, 0);
        buffer[n] = '\0';
        char str[50];
        strcpy(str, buffer);

        // Ordinamento della stringa
        for (i = 0; i < strlen(str) - 1; i++)
        {
            for (j = i + 1; j < strlen(str); j++)
            {
                if (str[i] > str[j])
                {
                    char tmp = str[i];
                    str[i] = str[j];
                    str[j] = tmp;
                }
            }
        }

        // Risposta al client
        strcpy(risposta, str);
        break;
    }
    case 5:
    {
        // Ricezione delle stringhe
        n = recv(connfd, buffer, 1024, 0);
        buffer[n] = '\0';
        char str1[50];
        char str2[50];
        strcpy(str1, buffer);
        strcpy(str2, buffer + strlen(str1));

        // Ricerca dei caratteri comuni
        int cont = 0;
        for (i = 0; i < strlen(str1); i++)
        {
            for (j = 0; j < strlen(str2); j++)
            {
                if (str1[i] == str2[j])
                {
                    risposta[cont] = str1[i];
                    cont++;
                    break;
                }
            }
        }
        risposta[cont] = '\0';
        break;
    }
    }

    // Invio della risposta al client
    send(connfd, risposta, strlen(risposta), 0);

    return 0;
}