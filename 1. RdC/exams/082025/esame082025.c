#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

int inviaByte(int fd, char *buffer, int numeroByte)
{
    int byteScritti = 0;
    int m = 0;
    while (byteScritti < numeroByte)
    {
        m = write(fd, buffer + byteScritti, numeroByte - byteScritti);
        byteScritti += m;
    }
}

int main()
{

    struct sitePort
    {
        char *sito;
        short int port;
    };

    struct sitePort mappa[2];

    mappa[0].sito = "www.sito1.com";
    mappa[0].port = 8888;

    mappa[1].sito = "www.sito2.com";
    mappa[1].port = 8889;

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
    address.sin_port = htons(9012);
    address.sin_addr.s_addr = INADDR_ANY;

    int s = bind(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (s != 0)
    {
        perror("bind fallita");
        exit(1);
    }
    else
    {
        printf("bind avvento con successo");
        fflush(stdout);
    }

    int l = listen(sockfd, 5);

    printf("il server è pronto per l'accept");
    fflush(stdout);
    while (1)
    {

        int clientSockId = accept(sockfd, NULL, NULL);

        int forkId = fork();
        if (forkId == 0)
        {
            printf("accept effettuata");

            char buffer[10000];

            printf("mi preparo a leggere\n");
            fflush(stdout);
            int n = 0;
            int lettoNomeHeader = 1;
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
            char *headerHost;
            char *contentLengthValue;
            int contentLength = 0;
            printf("stampo gli headers");
            for (int i = 0; i < headerIndex; i++)
            {
                if (strcmp(h[i].n, "Content-Length") == 0)
                {
                    sscanf(h[i].v, "%d", &contentLength);
                    contentLengthValue = h[i].v + 1;
                }
                if (strcmp(h[i].n, "Host") == 0)
                {
                    headerHost = h[i].v + 1;
                    // printf("%s:%s\n", h[i].n, h[i].v);
                }
            }

            for (int i = 0; i < strlen(headerHost); i++)
            {
                if (headerHost[i] == ':')
                    headerHost[i] = 0;
            }
            printf("\nheaderHost:%s\n\n", headerHost);

            // printf("content length = %d\n", contentLength);
            char *requestLine = buffer;
            printf("request line:%s", requestLine);

            char method[10], uri[100], version[10];

            sscanf(requestLine, "%s %s %s", method, uri, version);

            printf("method:%s\n", method);
            printf("uri:%s\n", uri);
            printf("version:%s\n", version);

            if (strcmp(method, "CONNECT") == 0)
                printf("metodo = connect");
            else
                printf("metodo <> connect");

            fflush(stdout);
            char response[1000] = "HTTP/1.1 200 OK\r\nTransfer-Encoding:chunked\r\n\r\n";

            if (strcmp(method, "GET") == 0)
            {
                if (strcmp(uri, "/") == 0)
                {
                    // sprintf(uri, "/index.html");
                }
                /*
    int j = 0;
    for(j; j < strlen(uri); j++){
        if(j > 0 && uri[j] == '/' && uri[j-1] == '/')
            break;
    }

    char *hostname = uri + j + 1;
    char *new_uri;

    new_uri = uri + j;
    j++;
     for(j; j < strlen(uri); j++){
         if(j > 0 && uri[j] == '/'){
             uri[j] = 0;
             new_uri = uri + j + 1;

             break;
         }
     }

    */
                int socket2 = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in address2;
                short int portaBackend = 0;
                for (int i = 0; i < 2; i++)
                {
                    if (strcmp(mappa[i].sito, headerHost) == 0)
                    {
                        portaBackend = mappa[i].port;
                    }
                }

                if (portaBackend == 0)
                {
                    char responseBadGateway[1000];
                    sprintf(responseBadGateway, "HTTP/1.1 502 Bad Gateway\r\n\r\n");
                    inviaByte(clientSockId, responseBadGateway, strlen(responseBadGateway));
                    exit(0);
                }
                address2.sin_family = AF_INET;
                address2.sin_port = htons(portaBackend); // big endian di 8080 (network byte order)

                printf("\n\nmi connetto alla porta:%d\n\n", portaBackend);

                char *ip = (char *)&address2.sin_addr.s_addr; // = *(unsigned int*) addr->h_addr;
                ip[0] = 127;
                ip[1] = 0;
                ip[2] = 0;
                ip[3] = 1;

                int c = connect(socket2, (struct sockaddr *)&address2, sizeof(address2));
                char request2[1000];
                sprintf(request2, "GET %s HTTP/1.1\r\nConnection:close\r\nHost:%s\r\n\r\n", uri, headerHost);
                printf("\n\nsto inviando:%s\n\n", request2);
                inviaByte(socket2, request2, strlen(request2));

                char buffer2[1000];
                int m = 0;
                while (m = read(socket2, buffer2, sizeof(buffer2) > 0))
                {
                    printf("risposta:%s\n", buffer2);
                    inviaByte(clientSockId, buffer2, m);
                }
            }
            else if (strcmp(method, "POST") == 0)
            {
                if (memcmp(uri, "/cgi-bin", 8) == 0)
                {
                    int pid = fork();
                    if (pid == 0)
                    {
                        char *queryString = NULL;

                        int i;
                        for (i = 0; uri[i] != 0 && uri[i] != '?'; i++)
                        {
                        }

                        if (uri[i] == '?')
                        {
                            uri[i] = 0;
                            printf("il valore della uri = %s\n\n", uri);
                            queryString = uri + i + 1;
                            printf("il valore della queryString = %s\n\n", queryString);
                            setenv("QUERY_STRING", queryString, 1);
                        }
                        dup2(clientSockId, 0); // stdin
                        dup2(clientSockId, 1); // stdout;
                        setenv("METHOD", "POST", 1);
                        setenv("ContentLength", contentLengthValue, 1);
                        printf("HTTP/1.1 200 OK\r\n\r\n");
                        execv(uri + 1, NULL);
                    }
                    else
                    {
                        int status;
                        waitpid(pid, &status, 0);
                    }
                    // printf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html>PAGINA NON TROVATA!</html");
                    // inviaByte(clientSockId, response, strlen(response));
                }
                else
                {

                    int m = 0;
                    char bufferFile[1024];

                    while ((m += read(clientSockId, bufferFile + m, sizeof(bufferFile))) < contentLength)
                    {
                    }
                    printf("buffer Body:%s\n", bufferFile);
                    inviaByte(clientSockId, response, strlen(response));
                }
            }
            else if (strcmp(method, "CONNECT") == 0)
            {
                printf("sono nella connect\n");
                fflush(stdout);
                char *port;
                int j;
                for (j = 0; uri[j] != ':'; j++)
                {
                }

                uri[j] = 0;
                port = uri + j + 1;

                printf("address a cui connettersi:%s\n", uri);
                int portInt = atoi(port);
                printf("porta a cui connettersi:%d da stringa = %s\n", portInt, port);
                fflush(stdout);

                int socket2 = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in address2;

                address2.sin_family = AF_INET;
                address2.sin_port = htons(portInt); // big endian di 8080 (network byte order)

                struct hostent *addr = gethostbyname(uri);
                address2.sin_addr.s_addr = *(unsigned int *)addr->h_addr;

                int c = connect(socket2, (struct sockaddr *)&address2, sizeof(address2));

                char buffer2[1000];
                sprintf(buffer2, "HTTP/1.1 200 Established\r\n\r\n");
                inviaByte(clientSockId, buffer2, strlen(buffer2));

                int fork2 = fork();

                if (fork2 == 0)
                {
                    char bufferClient[1000];
                    int m = 0;
                    while ((m = read(socket2, bufferClient, sizeof(bufferClient))) > 0)
                    {

                        inviaByte(clientSockId, bufferClient, m);
                    }
                }
                else
                {
                    char bufferClient[1000];
                    int m = 0;
                    while ((m = read(clientSockId, bufferClient, sizeof(bufferClient))) > 0)
                    {
                        inviaByte(socket2, bufferClient, m);
                    }
                }
            }
            else
            {
                sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
                inviaByte(clientSockId, response, strlen(response));
            }

            close(clientSockId);
            return 0;
        }
        else
        {
            close(clientSockId);
        }
    }
}
