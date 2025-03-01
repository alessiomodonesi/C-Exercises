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
    struct sockaddr_in serv_addr;
    char buffer[1024];

    // Creazione del socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Errore nell'apertura del socket\n");
        return 1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1234);

    // Converti l'indirizzo IP in una forma di servizio
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("Errore nell'indirizzo IP\n");
        return 1;
    }

    // Connessione al server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Errore nella connessione\n");
        return 1;
    }

    // scelta dell'esercizio
    int num;
    printf("Inserire il numero dell'esercizio da eseguire (1-5): ");
    scanf("%d", &num);
    buffer[0] = num;

    // invio dell'esercizio da eseguire
    send(sockfd, buffer, strlen(buffer), 0);

    // invio dei dati necessari all'esecuzione dell'esercizio
    switch (num)
    {
    case 1:
    {
        char str[50];
        printf("Inserire la stringa da controllare: ");
        scanf("%s", str);
        strcpy(buffer, str);
        send(sockfd, buffer, strlen(buffer), 0);
        break;
    }
    case 2:
    {
        char str[50];
        char c;
        printf("Inserire la stringa da controllare: ");
        scanf("%s", str);
        printf("Inserire il carattere da cercare: ");
        scanf(" %c", &c);
        strcpy(buffer, str);
        buffer[strlen(str)] = c;
        send(sockfd, buffer, strlen(buffer), 0);
        break;
    }
    case 3:
    {
        char str[50];
        printf("Inserire la stringa da controllare: ");
        scanf("%s", str);
        strcpy(buffer, str);
        send(sockfd, buffer, strlen(buffer), 0);
        break;
    }
    case 4:
    {
        char str[50];
        printf("Inserire la stringa da ordinare: ");
        scanf("%s", str);
        strcpy(buffer, str);
        send(sockfd, buffer, strlen(buffer), 0);
        break;
    }
    case 5:
    {
        char str1[50];
        char str2[50];
        printf("Inserire la prima stringa: ");
        scanf("%s", str1);
        printf("Inserire la seconda stringa: ");
        scanf("%s", str2);
        strcpy(buffer, str1);
        strcat(buffer, str2);
        send(sockfd, buffer, strlen(buffer), 0);
        break;
    }
    }

    // Risposta dal server
    int n = recv(sockfd, buffer, 1024, 0);
    buffer[n] = '\0';
    printf("Risposta dal server: %s\n", buffer);

    return 0;
}