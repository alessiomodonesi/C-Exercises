#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DIM 1024
#define PORT 8080

int main()
{
    struct sockaddr_in data;
    data.sin_family = AF_INET;
    data.sin_addr.s_addr = htonl(INADDR_ANY);
    data.sin_port = htons(PORT);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    connect(socket_fd, (struct sockaddr *)&data, sizeof(data));

    char string[DIM];
    printf("Inserisci la tua stringa: ");
    scanf("%s", string);

    send(socket_fd, string, strlen(string), 0);
    close(socket_fd);
}