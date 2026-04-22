#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

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

    int l = listen(sockfd, 5);

    printf("il server è pronto per l'accept\n");
    fflush(stdout);
    int clientSockId = accept(sockfd, NULL, NULL);

    char buffer[10000];

    printf("mi preparo a leggere\n");
    fflush(stdout);
    int n = 0;
    int lettoNomeHeader = 0;
    int byteLetti = 0;
    int headerIndex = 0;
    while ((n += read(clientSockId, buffer + n, 1)) > 0)
    {
        printf("%c", buffer[n - 1]);
        if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
        {
            if (buffer[n - 4] == 0)
            {
                // body = buffer + byteLetti + 1;
                break;
            }
            lettoNomeHeader = 0;
            buffer[byteLetti - 1] = 0;
            h[headerIndex].n = buffer + byteLetti + 1;
        }
        else if (!lettoNomeHeader && buffer[byteLetti] == ':')
        {
            lettoNomeHeader = 1;
            buffer[byteLetti] = 0;
            h[headerIndex++].v = buffer + byteLetti + 1;
        }
    }

    char response[] = "HTTP/1.1 200 OK\r\n\r\n<html><body>CIAO CLIENT!</body></html>";

    int byteScritti = 0;
    int m = 0;
    while (byteScritti < strlen(response))
    {
        m = write(clientSockId, response + byteScritti, strlen(response) - byteScritti);
        byteScritti += m;
    }
}
