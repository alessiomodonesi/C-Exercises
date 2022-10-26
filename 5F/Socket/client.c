#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DIM 1024
#define PORT 8080

int main()
{
    struct sockaddr_in address;
    int client, valread, sock = 0;
    char buffer[DIM] = {0};
    char string[DIM] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nsocket failed\n");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
    {
        printf("\ninvalid address\n");
        return -1;
    }

    if ((client = connect(sock, (struct sockaddr *)&address,
                          sizeof(address))) < 0)
    {
        printf("\nconnection failed\n");
        return -1;
    }

    while (1)
    {
        printf("Write: ");
        scanf("%s", string);

        if (strcmp(string, "esc") != 0)
            send(sock, string, strlen(string), 0);
        else
            return 0;

        valread = read(sock, buffer, 1024);
        printf("Read: %s\n", buffer);
    }
    close(client);
}