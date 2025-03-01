#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Numero argomenti sbagliato");
        exit(1);
    }

    int p1p2[2], p2p3[2], p3p0[2];
    double totale = 0;
    char buffer[1], *ptr, str[100];

    pipe(p1p2);
    int p1 = fork();
    if (p1 == 0)
    {
        close(1); //output per p2
        dup(p1p2[1]);
        close(p1p2[1]);
        close(p1p2[0]);

        execl("/bin/cat", "cat", argv[1], NULL);
        return -1;
    }

    pipe(p2p3);
    int p2 = fork();
    if (p2 == 0)
    {
        close(0); //input da p1 
        dup(p1p2[0]);
        close(p1p2[0]);
        close(p1p2[1]);

        close(1); //output per p3
        dup(p2p3[1]);
        close(p2p3[1]);
        close(p2p3[0]);

        execl("/usr/bin/awk", "awk", "{print $4}", NULL);
        return -1;
    }

    pipe(p3p0);
    int p3 = fork();
    if (p3 == 0)
    {
        close(p1p2[0]);
        close(p1p2[1]);

        close(0); //input da p2
        dup(p2p3[0]);
        close(p2p3[0]);
        close(p2p3[1]);

        close(1); //output per p0
        dup(p3p0[1]);
        close(p3p0[1]);
        close(p3p0[0]);

        execl("/usr/bin/tail", "tail", "-n", "+2", NULL);
        return -1;
    }

    close(p1p2[1]);
    close(p1p2[0]);
    close(p2p3[0]);
    close(p2p3[0]);
    close(p3p0[1]);

    while (read(p3p0[0], buffer, 1) > 0)
    {
        strncat(str, &buffer[0], sizeof(buffer[0]));
        if (buffer[0] == '\n')
        {
            printf("tempo parziale: %s", str);
            totale = totale + strtod(str, &ptr);
            str[0] = '\0';
        }
    }

    close(p3p0[0]);
    printf("\ntempo totale: %.2lf\n", totale);
    return 0;
}