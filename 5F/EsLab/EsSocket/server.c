#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DIM 1024
#define PORT 8080

int IsPal(char *str)
{
    for (int i = 0, _i = strlen(str) - 1; i < strlen(str) / 2; i++)
    {
        if (str[i] != str[_i--])
            return 0;
    }
    return 1;
}

int SearchChar(char *str, char *c)
{
    int cont = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (strcmp(&str[i], c))
            cont++;
    }
    return cont;
}

int FindLetter(char *str, int flag)
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

void SortString(char *str)
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

char CommonLetter(char *str1, char *str2, char *res)
{
    int x = 0;
    for (int i = 0; i < strlen(str1); i++)
    {
        for (int j = 0; j < strlen(str2); j++)
        {
            if (str1[i] == str2[j])
            {
                res[x++] = str1[i];
            }
        }
    }
    return *res;
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
    printf("\nlistening server...");

    while (1)
    {
        char buffer[DIM] = {0}, common[DIM] = {0};
        fflush(stdout);
        int length = sizeof(client);
        int socket_client = accept(socket_server, (struct sockaddr *)&client, (socklen_t *)&length);

        // es 1
        char string1[DIM] = {0};
        read(socket_client, string1, sizeof(string1));
        if (IsPal(string1) == 1)
            sprintf(buffer, "\nthe string is palindrome");
        else
            sprintf(buffer, "\nthe string is NOT palindrome");
        write(socket_client, buffer, sizeof(buffer));

        // es 2
        char c[1] = {0};
        read(socket_client, c, sizeof(c));
        int num = SearchChar(string1, c);
        sprintf(buffer, "\nchar founded %d times", num / 4);
        write(socket_client, buffer, sizeof(buffer));

        // es 3
        int con = FindLetter(string1, 0);
        int voc = FindLetter(string1, 1);
        sprintf(buffer, "\nconsonants: %d \nvowels: %d", con, voc);
        write(socket_client, buffer, sizeof(buffer));

        // es 4
        SortString(string1);
        sprintf(buffer, "\nsorted string: %s", string1);
        write(socket_client, buffer, sizeof(buffer));

        // es 5
        char string2[DIM] = {0};
        read(socket_client, string2, sizeof(string2));
        CommonLetter(string1, string2, common);
        sprintf(buffer, "\ncommon letters: %s", common);
        write(socket_client, buffer, sizeof(buffer));

        close(socket_client);
    }
    close(socket_server);
}