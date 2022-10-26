#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DIM 1024
#define PORT 8080

int main()
{
    struct sockaddr_in data;
    int addrlen = sizeof(data);
    data.sin_family = AF_INET;
    data.sin_addr.s_addr = INADDR_ANY;
    data.sin_port = htons(PORT);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(socket_fd, (struct sockaddr *)&data, sizeof(data));
    listen(socket_fd, 10);

    while (1)
    {
        printf("\nServer in ascolto...");
        fflush(stdout);
        int new_socket = accept(socket_fd, (struct sockaddr *)&data, (socklen_t *)&addrlen);

        char buffer[DIM] = {0};
        read(new_socket, buffer, 1024);
        printf("\nRead: %s\n", buffer);

        close(new_socket);
    }
    shutdown(socket_fd, SHUT_RDWR);
}