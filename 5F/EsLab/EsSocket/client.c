#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DIM 1024
#define PORT 8080

void FlushBuffer(char *arr)
{
    for (int i = 0; i < DIM + 1; i++)
        arr[i] = '\0';
}

int main()
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    connect(socket_client, (struct sockaddr *)&server, sizeof(server));

    char str1[DIM], str2[DIM], buffer[DIM], c[1];
    printf("\nSend the 1st string to the server: ");
    scanf("%s", str1);
    write(socket_client, str1, strlen(str1));

    printf("\nSend the 2nd string to the server: ");
    scanf("%s", str2);
    write(socket_client, str2, strlen(str2));

    printf("\nSend a char to the server: ");
    scanf("%s", c);
    write(socket_client, c, strlen(c));

    // es 1
    read(socket_client, buffer, DIM);
    printf("\n%s", buffer);
    FlushBuffer(buffer);

    // es 2
    read(socket_client, buffer, DIM);
    printf("\n%s", buffer);
    FlushBuffer(buffer);

    // es 3
    read(socket_client, buffer, DIM);
    printf("\n%s", buffer);
    FlushBuffer(buffer);

    // es 4
    read(socket_client, buffer, DIM);
    printf("\n%s", buffer);
    FlushBuffer(buffer);

    // es 5
    read(socket_client, buffer, DIM);
    printf("\n%s", buffer);
    close(socket_client);
}