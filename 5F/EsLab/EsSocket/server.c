#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DIM 1024
#define PORT 8080

int Es1(char *str) // check palindroma
{
    for (int i = 0, _i = strlen(str) - 1; i < strlen(str) / 2; i++)
    {
        if (str[i] != str[_i--])
            return 0;
    }
    return 1;
}

int Es2(char *str, char *c) // cerca un char
{
    int cont = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (strcmp(&str[i], c))
            cont++;
    }
    return cont;
}

int Es3(char *str, int flag) // conta vocali e consonanti
{
    int con = 0, voc = 0;
    char array[5] = {'a', 'e', 'i', 'o', 'u'};
    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (str[i] == array[j])
                voc++;
            else
                con++;
        }
    }
    if (flag == 0)
        return con;
    else
        return voc;
}

void Es4(char *str) // ordina la stringa
{
    char tmp;
    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = 0; j < (strlen(str) - 1); j++)
        {
            if (str[j] > str[j + 1])
            {
                tmp = str[j];
                str[j] = str[j + 1];
                str[j + 1] = tmp;
            }
        }
    }
}

char *Es5(char *str) // cerca le lettere comuni tra 2 stringhe
{
    char res[DIM];
    return res;
}

int main()
{
    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    int socket_server = socket(AF_INET, SOCK_STREAM, 0);
    bind(socket_server, (struct sockaddr *)&server, sizeof(server));
    listen(socket_server, 10);

    while (1)
    {
        printf("\nServer in ascolto...");
        fflush(stdout);
        int length = sizeof(client);
        int socket_client = accept(socket_server, (struct sockaddr *)&client, (socklen_t *)&length);

        char c;
        char common[DIM];
        char string[DIM];
        char buffer[DIM];
        read(socket_client, string, DIM);
        read(socket_client, c, strlen(c));

        strcat(buffer, string);
        if (Es1(string) == 1)
            strcat(buffer, ", palindroma");
        else
            strcat(buffer, ", non palindroma");
        int res = Es2(string, c);
        int con = Es3(string, 0);
        int voc = Es3(string, 1);
        Es4(string);
        Es5(string);

        write(socket_client, buffer, strlen(buffer));
        close(socket_client);
    }
    close(socket_server);
}