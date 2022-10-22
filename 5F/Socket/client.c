#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
    struct sockaddr_in serv_addr;
    int client, valread, sock = 0;
    char buffer[1024] = {0};
    char string[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nsocket failed\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\ninvalid address\n");
        return -1;
    }

    if ((client = connect(sock, (struct sockaddr *)&serv_addr,
                          sizeof(serv_addr))) < 0)
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