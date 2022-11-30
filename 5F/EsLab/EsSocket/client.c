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

    char str1[DIM], str2[DIM];
    printf("\nSend the 1st string to the server: ");
    scanf("%s", str1);
    write(socket_client, str1, strlen(str1));

    printf("\nSend the 2nd string to the server: ");
    scanf("%s", str2);
    write(socket_client, str2, strlen(str2));

    char c;
    printf("\nSend a char to the server: ");
    scanf("%s", c);
    write(socket_client, c, strlen(c));

    // results
    char buffer[DIM] = {0};
    sleep(3);
    read(socket_client, buffer, DIM);
    printf("\n%s", buffer);
    close(socket_client);
}