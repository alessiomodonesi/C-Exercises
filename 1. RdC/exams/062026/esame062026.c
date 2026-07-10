#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void inviaByte(int fd, char *buffer, int numeroByte)
{
    int m = 0;
    int byteScritti = 0;
    while (byteScritti < numeroByte)
    {
        m = write(fd, buffer + byteScritti, numeroByte - byteScritti);
        if (m < 0)
        {
            break;
        }
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
    address.sin_port = htons(8088);
    address.sin_addr.s_addr = INADDR_ANY;

    int s = bind(sockfd, (struct sockaddr *)&address, sizeof(address));
    if (s != 0)
    {
        perror("Bind fallita\n");
        exit(1);
    }
    else
    {
        printf("Bind riuscita\n");
        fflush(stdout);
    }

    int l = listen(sockfd, 5);
    printf("Il server è pronto per l'accept\n");
    fflush(stdout);

    while (1)
    {
        int clientSockID = accept(sockfd, NULL, NULL);

        int forkId = fork();
        if (forkId == 0)
        {
            printf("Accept effettuata\n");

            char buffer[100000];
            printf("Mi preparo a leggere\n");
            fflush(stdout);

            int n = 0;
            int indiceHeader = 0;
            int lettoNomeHeader = 0;

            while ((n += read(clientSockID, buffer + n, 1)) > 0)
            {

                if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
                {
                    if (buffer[n - 4] == 0)
                    {
                        break;
                    }
                    lettoNomeHeader = 0;
                    buffer[n - 2] = 0;
                    h[indiceHeader].n = buffer + n;
                }
                else if (!lettoNomeHeader && buffer[n - 1] == ':')
                {
                    buffer[n - 1] = 0;
                    lettoNomeHeader = 1;
                    h[indiceHeader++].v = buffer + n;
                }
            }

            printf("Stampo gli headers\n");
            int contentLength = 0;
            char *referer = NULL;
            char *secFetchSite = NULL;
            for (int i = 0; i < indiceHeader; i++)
            {
                printf("%s: %s\n", h[i].n, h[i].v);
                if (strcmp(h[i].n, "Content-Length") == 0)
                {
                    sscanf(h[i].v, "%d", &contentLength);
                }

                if (strcmp(h[i].n, "Referer") == 0)
                {
                    referer = h[i].v;
                }
                else if (strcmp(h[i].n, "Sec-Fetch-Site") == 0)
                {
                    secFetchSite = h[i].v;
                }
            }
            printf("Content length = %d\n", contentLength);

            char *requestLine = buffer;
            printf("Request line: %s\n", requestLine);

            char method[10], uri[100], version[10];
            sscanf(requestLine, "%s %s %s", method, uri, version);
            printf("Method: %s, URI: %s, Version: %s\n", method, uri, version);

            char response[1024] = "HTTP/1.1 200 OK\r\n\r\n";

            if (strcmp(method, "GET") == 0)
            {

                if (strcmp(uri, "/") == 0)
                {
                    sprintf(uri, "/index.html");
                }

                int fd = open(uri + 1, O_RDONLY);
                if (fd < 0)
                {
                    sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html><body>Pagina non trovata</body></html>");
                    inviaByte(clientSockID, response, (int)strlen(response));
                }
                else
                {
                    int autorizzato = 0;

                    if (strcmp(uri, "/private.html") == 0)
                    {
                        if (referer != NULL && strstr(referer, "partner1.com") != NULL)
                        {
                            autorizzato = 1;
                        }
                        else if (secFetchSite != NULL)
                        {
                            if (strstr(secFetchSite, "same-origin") != NULL || strstr(secFetchSite, "same-site") != NULL)
                            {
                                autorizzato = 1;
                            }
                        }

                        if (autorizzato == 0)
                        {
                            sprintf(response, "HTTP/1.1 403 Forbidden\r\n\r\n<html><body>Accesso negato</body></html>");
                            inviaByte(clientSockID, response, (int)strlen(response));
                            close(fd);
                            close(clientSockID);
                            return 0;
                        }
                    }

                    inviaByte(clientSockID, response, (int)strlen(response));

                    int m = 0;
                    char bufferFile[1024];
                    while ((m = read(fd, bufferFile, sizeof(bufferFile))) > 0)
                    {
                        inviaByte(clientSockID, bufferFile, m);
                    }
                    close(fd);
                }
            }
            else if (strcmp(method, "POST") == 0)
            {
                int m = 0;
                char bufferFile[10000] = {0};

                while (m < contentLength && m < (int)sizeof(bufferFile) - 1)
                {
                    int r = read(clientSockID, bufferFile + m, sizeof(bufferFile) - 1 - m);
                    if (r <= 0)
                    {
                        break;
                    }
                    m += r;
                }

                printf("Buffer body ricevuto:\n%s\n", bufferFile);
                sprintf(response, "HTTP/1.1 200 OK\r\n\r\n<html><body>Dati POST ricevuti correttamente</body></html>");
                inviaByte(clientSockID, response, (int)strlen(response));
            }
            else
            {
                sprintf(response, "HTTP / 1.1 405 Method Not Allowed\r\n\r\n");
                inviaByte(clientSockID, response, (int)strlen(response));
            }

            close(clientSockID);
            return 0;
        }
        else
        {
            close(clientSockID);
        }
    }
}