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

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;

    int s = bind(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (s != 0)
    {
        perror("bind failed");
    }
    else
    {
        printf("bind successful\n");
    }

    int l = listen(sockfd, 5);

    int clientSockID = accept(sockfd, NULL, NULL);

    char buffer[100];

    printf("Client connected\n");
    read(clientSockID, buffer, sizeof(buffer) - 1);

    printf("Received message: %s\n", buffer);

    char response[] = "Server response";

    int toWriteChars = strlen(response);
    int writedChars = 0, writeCount = 0;

    while (writedChars < toWriteChars)
    {
        writeCount = write(clientSockID, response + writedChars, strlen(response) - writedChars);
        writedChars += writeCount;
    }

    printf("Writed chars: %d\n", writeCount);
}