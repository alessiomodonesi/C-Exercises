#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DIM 1024
#define PORT 8080

int main()
{
    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    int socket_server = socket(AF_INET, SOCK_STREAM, 0);
    bind(socket_server, (struct sockaddr *)&server, sizeof(server));
    listen(socket_server, 10);

    while (1)
    {
        printf("\nServer in ascolto...");
        fflush(stdout);
        int length = sizeof(client);
        int socket_client = accept(socket_server, (struct sockaddr *)&client, (socklen_t *)&length);

        char buffer[DIM] = {0};
        read(socket_client, buffer, DIM);
        printf("\nRead: %s\n", buffer);
        close(socket_client);
    }
    close(socket_server);
}