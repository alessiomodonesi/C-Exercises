#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(8080); // big endian di 8080 (network byte order)

    // address.sin_addr.s_addr = inet_addr("127.0.0.1");
    char *ip = (char *)&address.sin_addr.s_addr;
    ip[0] = 127;
    ip[1] = 0;
    ip[2] = 0;
    ip[3] = 1;

    int c = connect(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (c == 0)
    {
        printf("Connected to server\n");
    }
    else
    {
        perror("Connection failed");
        exit(1);
    }

    char buffer[] = "Hello, Server!";
    write(sockfd, buffer, strlen(buffer));

    char response[1000];
    read(sockfd, response, sizeof(response));

    printf("Server says:%s\n", response);
    // close(sockfd);
}
