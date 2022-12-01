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
    char buffer[DIM] = {0};

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    connect(socket_client, (struct sockaddr *)&server, sizeof(server));

    // es 1
    char str1[DIM] = {0};
    printf("\nsend the 1st string: ");
    scanf("%s", str1);
    write(socket_client, str1, sizeof(str1));
    read(socket_client, buffer, sizeof(buffer));
    printf("%s", buffer);

    // es 2
    char c[1] = {0};
    printf("\nsend a char: ");
    scanf("%s", c);
    write(socket_client, c, sizeof(c));
    read(socket_client, buffer, sizeof(buffer));
    printf("%s", buffer);

    // es 3
    read(socket_client, buffer, sizeof(buffer));
    printf("%s", buffer);

    // es 4
    read(socket_client, buffer, sizeof(buffer));
    printf("%s", buffer);

    // es 5
    char str2[DIM] = {0};
    printf("\nsend the 2nd string: ");
    scanf("%s", str2);
    write(socket_client, str2, sizeof(str2));
    read(socket_client, buffer, sizeof(buffer));
    printf("%s", buffer);

    close(socket_client);
}