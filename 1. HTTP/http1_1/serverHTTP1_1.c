#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void sendByte(int fd, char *buffer, int numeroByte)
{
    int m = 0;
    int byteScritti = 0;
    while (byteScritti < numeroByte)
    {
        m = write(fd, buffer + byteScritti, numeroByte - byteScritti);
        byteScritti += m;
    }
}

int main()
{
    struct header
    {
        char *n;
        char *v;
    };

    struct header h[100];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8081);
    address.sin_addr.s_addr = INADDR_ANY;

    int s = bind(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (s != 0)
    {
        perror("bind fallita\n");
        exit(1);
    }
    else
    {
        printf("bind avvento con successo\n");
        fflush(stdout);
    }

    int l = listen(sockfd, 5); // posso accettare 5 client in parallelo

    printf("il server è pronto per l'accept\n");
    fflush(stdout);

    while (1)
    {
        int clientSockId = accept(sockfd, NULL, NULL);

        int forkId = fork(); // creo un processo figlio per gestire la richiesta del client, così il processo padre può continuare ad accettare altre richieste
        if (forkId == 0)     // entro nel processo figlio, gestisco la richiesta del client
        {
            printf("accept effettuata\n");

            char buffer[1024];

            printf("mi preparo a leggere\n");
            fflush(stdout);

            int n = 0;
            int lettoNomeHeader = 0;
            int byteLetti = 0;
            int headerIndex = 0;
            while ((n += read(clientSockId, buffer + n, 1)) > 0)
            {
                // printf("%c", buffer[n - 1]);
                if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
                {
                    if (buffer[n - 4] == 0)
                    {
                        // body = buffer + byteLetti + 1;
                        break;
                    }
                    lettoNomeHeader = 0;
                    buffer[n - 2] = 0;
                    h[headerIndex].n = buffer + n;
                }
                else if (!lettoNomeHeader && buffer[n - 1] == ':')
                {
                    lettoNomeHeader = 1;
                    buffer[n - 1] = 0;
                    h[headerIndex++].v = buffer + n;
                }
            }

            printf("stampo gli headers\n");
            for (int i = 0; i < headerIndex; i++)
                printf("%s:%s\n", h[i].n, h[i].v);

            char *requestLine = buffer; // GET /index.html HTTP/1.1
            printf("request line: %s\n", requestLine);

            char method[10], uri[100], version[10];
            sscanf(requestLine, "%s %s %s", method, uri, version); // GET /index.html HTTP/1.1

            // se il metodo è POST, PUT, DELETE, ecc, allora non è supportato > 405 Method Not Allowed
            printf("method: %s\n", method);
            printf("uri: %s\n", uri);
            printf("version: %s\n", version);

            // char response[1024] = "HTTP/1.1 200 OK\r\n\r\n<html><body>Hello client!</body></html>";
            char response[1024] = "HTTP/1.1 200 OK\r\n\r\n";

            if (strcmp(method, "GET") == 0) // accetto solo GET, se è un altro metodo allora 405 Method Not Allowed
            {
                if (strcmp(uri, "/") == 0)
                    sprintf(uri, "/index.html");

                int fd = open(uri + 1, O_RDONLY); // uri + 1 per saltare la barra iniziale

                if (fd < 0)
                {
                    // chiamo la funzione definita sopra per inviare la risposta al client
                    sendByte(clientSockId, response, strlen(response));
                    sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html><body>File not found!</body></html>");
                }
                else
                {
                    sendByte(clientSockId, response, strlen(response));

                    int m = 0;
                    char bufferFile[1024];

                    while ((m = read(fd, bufferFile, sizeof(bufferFile))) > 0)
                    {
                        sendByte(clientSockId, bufferFile, m);
                    }
                }

                close(fd);
            }
            else
            {
                sendByte(clientSockId, response, strlen(response));
                sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
            }

            close(clientSockId);
            return 0;
        }
        else
            close(clientSockId);
    }
}