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
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    connect(socket_client, (struct sockaddr *)&server, sizeof(server));

    char string[DIM];
    printf("\nInserisci la tua stringa: ");
    scanf("%s", string);
    printf("\n");

    write(socket_client, string, strlen(string));
    close(socket_client);
}