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
    struct sockaddr_in address;
    int server, valread, new_socket;
    int addrlen = sizeof(address), opt = 1;
    char buffer[DIM] = {0};
    char string[DIM] = {0};

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(1);
    }

    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(1);
    }
    if (listen(server, 3) < 0)
    {
        perror("listen");
        exit(1);
    }
    if ((new_socket = accept(server, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(1);
    }

    while (1)
    {
        valread = read(new_socket, buffer, 1024);
        printf("Read: %s\n", buffer);

        printf("Write: %s", string);
        scanf("%s", string);

        if (strcmp(string, "esc") != 0)
            send(new_socket, string, strlen(string), 0);
        else
            return 0;
    }
    close(new_socket);
    shutdown(server, SHUT_RDWR);
}