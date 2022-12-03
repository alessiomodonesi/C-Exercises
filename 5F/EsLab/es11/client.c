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

    // send str1 to the server
    char str1[DIM];
    printf("send the 1st string: ");
    scanf("%s", str1);
    write(socket_client, str1, sizeof(str1));

    // send a char to the server
    char c;
    printf("send a char: ");
    scanf(" %c", &c);
    write(socket_client, &c, sizeof(c));

    // send str2 to the server
    char str2[DIM];
    printf("send the 2nd string: ");
    scanf("%s", str2);
    write(socket_client, str2, sizeof(str2));

    // response by the server
    char buffer[DIM];
    read(socket_client, buffer, DIM);
    printf("\n%s\n", buffer);

    close(socket_client);
}