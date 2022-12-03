#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DIM 1024
#define PORT 8080

int Palindrome(char *str)
{
    for (int i = 0, _i = strlen(str) - 1; i < strlen(str) / 2; i++)
    {
        if (str[i] != str[_i--])
            return 0;
    }
    return 1;
}

int SearchChar(char *str, char c)
{
    int cont = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == c)
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

char *SortString(char *str1, char *str3)
{
    char tmp;
    strcpy(str3, str1);
    for (int i = 0; i < strlen(str3); i++)
    {
        for (int j = 0; j < (strlen(str3) - 1); j++)
        {
            if (str3[j] > str3[j + 1])
            {
                tmp = str3[j];
                str3[j] = str3[j + 1];
                str3[j + 1] = tmp;
            }
        }
    }
    return str3;
}

char *CommonLetter(char *str1, char *str2, char *str4)
{
    int x = 0;
    for (int i = 0; i < strlen(str1); i++)
    {
        for (int j = 0; j < strlen(str2); j++)
        {
            if (str1[i] == str2[j])
            {
                str4[x++] = str1[i];
            }
        }
    }
    return str4;
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
        fflush(stdout);
        char buffer[DIM];
        int length = sizeof(client);
        int socket_client = accept(socket_server, (struct sockaddr *)&client, (socklen_t *)&length);

        // get str1 by the client
        char str1[DIM];
        read(socket_client, str1, DIM);

        // get a char by the server
        char c;
        read(socket_client, &c, DIM);

        // get str2 by the client
        char str2[DIM];
        read(socket_client, str2, DIM);

        for (int i = 0; i < DIM + 1; i++)
            buffer[i] = '\0';

        sprintf(buffer, "Results: \n");

        // es 1
        if (Palindrome(str1) == 1)
            sprintf(buffer + strlen(buffer), "%s is palindrome \n", str1);
        else
            sprintf(buffer + strlen(buffer), "%s is NOT palindrome \n", str1);

        // es 2
        int num = SearchChar(str1, c);
        sprintf(buffer + strlen(buffer), "%c founded %d times \n", c, num / 4);

        // es 3
        int con = FindLetter(str1, 0);
        int voc = FindLetter(str1, 1);
        sprintf(buffer + strlen(buffer), "%d consonants \n%d vowels \n", con, voc);

        // es 4
        char str3[DIM] = {*SortString(str1, str3)};
        sprintf(buffer + strlen(buffer), "sort(%s): %s \n", str1, str3);

        // es 5
        char str4[DIM] = {*CommonLetter(str1, str2, str4)};
        sprintf(buffer + strlen(buffer), "common letters: %s \n", str4);

        // response to the client
        write(socket_client, buffer, sizeof(buffer));
        close(socket_client);
    }
    close(socket_server);
}