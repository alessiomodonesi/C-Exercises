#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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

            char *user_agent;
            int contentLength = 0;
            printf("stampo gli headers");
            for (int i = 0; i < headerIndex; i++)
            {
                if (strcmp(h[i].n, "User-Agent") == 0)
                {
                    user_agent = h[i].v;
                }

                // printf("%s:%s\n", h[i].n, h[i].v);
            }

            printf("userAgent = %s\n\n\n", user_agent);

            char *requestLine = buffer;
            printf("request line:%s", requestLine);

            char method[10], uri[100], version[10];

            sscanf(requestLine, "%s %s %s", method, uri, version);

            printf("method:%s\n", method);
            printf("uri:%s\n", uri);
            printf("version:%s\n", version);

            char response[1000] = "HTTP/1.1 200 OK\r\nTransfer-Encoding:chunked\r\n\r\n";
            char *user_agent_chrome = " Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/147.0.0.0 Safari/537.36";
            char *user_agent_curl = "curl";
            if (strcmp(user_agent, user_agent_chrome) == 0)
            {

                printf("ho letto chrome\n\n");
                sprintf(response, "HTTP/1.1 406 Not Acceptable\r\n\r\n");
                inviaByte(clientSockId, response, strlen(response));
                close(clientSockId);
            }
            else
            {
                if (strcmp(method, "GET") == 0)
                {
                    if (strcmp(uri, "/") == 0)
                    {
                        sprintf(uri, "/index.html");
                    }

                    int fd = open(uri + 1, O_RDONLY);
                    if (fd < 0)
                    {
                        inviaByte(clientSockId, response, strlen(response));
                        sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html>PAGINA NON TROVATA!</html>");
                    }
                    else
                    {

                        inviaByte(clientSockId, response, strlen(response));
                        int m = 0;
                        char bufferFile[1024];

                        while ((m = read(fd, bufferFile, sizeof(bufferFile))) > 0)
                        {
                            char chunkSize[100];
                            sprintf(chunkSize, "%x\r\n", m);
                            printf("sto inviando m = %d, 0x%s byte ", m, chunkSize);
                            inviaByte(clientSockId, chunkSize, strlen(chunkSize));
                            inviaByte(clientSockId, bufferFile, m);
                            inviaByte(clientSockId, "\r\n", 2);
                        }
                        inviaByte(clientSockId, "0\r\n\r\n", 5);
                    }
                }
                else if (strcmp(method, "POST") == 0)
                {
                    int m = 0;
                    char bufferFile[1024];

                    while ((m += read(clientSockId, bufferFile + m, sizeof(bufferFile))) < contentLength)
                    {
                    }
                    printf("buffer Body:%s\n", bufferFile);
                    inviaByte(clientSockId, response, strlen(response));
                }
                else
                {
                    sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
                    inviaByte(clientSockId, response, strlen(response));
                }

                close(clientSockId);
                return 0;
            }
        }
        else
        {
            close(clientSockId);
        }
    }
}
