#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
    struct header // ricorda: l'ordine della variabili è importante
    {
        char *n; // header name
        char *v; // header value
    };

    struct header h[100];
    char *statusLine;
    char *body;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(80); // big endian di 8080 (network byte order)

    char *ip = (char *)&address.sin_addr.s_addr;

    ip[0] = 217, ip[1] = 61, ip[2] = 15, ip[3] = 222; // www.example.it

    int c = connect(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (c == 0)
        printf("connection established\n");
    else
    {
        perror("connection failed");
        exit(1);
    }

    // su Linux è possibile inviare una richiesta HTTP con telnet, basta scrivere "telnet www.example.com 80" e
    // poi scrivere la richiesta HTTP (ad esempio "GET / HTTP/1.1\r\nHost:www.example.com\r\nConnection:keep-alive\r\n\r\n")

    // char buffer[] = "GET / HTTP/1.1\r\nHost:www.example.com\r\nConnection:keep-alive\r\n\r\n"; // ora in versione 1.1
    // Connection:close -> quando il server ha finito di inviare cose chiude la connessione (come nella 1.0)
    // Connection:keep-alive -> quando il server ha finito di inviare cose non chiude la connessione
    // ovviamente dopo l'inserimento di eventuali headers devo mettere CRLF CRLF

    char buffer[] = "GET / HTTP/1.1\r\nHost:www.example.it\r\nConnection:keep-alive\r\n\r\n";

    int m = 0;
    int bytesWritten = 0;
    while (bytesWritten < strlen(buffer))
    {
        m = write(sockfd, buffer + bytesWritten, strlen(buffer) - bytesWritten);
        bytesWritten += m;
    }

    //  printf("written %d bytes\n", bytesWritten);

    char response[1000000];

    int n = 0;
    int bytesRead = 0;
    while ((n = read(sockfd, response + bytesRead, 1)) > 0)
    {
        bytesRead += n;
    }

    // read(sockfd, response + bytesRead, sizeof(response) - bytesRead);
    /*
        while ((n = read(sockfd, response + bytesRead, sizeof(response) - bytesRead)) > 0)
        {
            printf("%s", response);
            printf("read %d bytes\n", n);
            bytesRead += n;
            printf("total bytes read: %d\n", bytesRead);
        }
    */

    statusLine = response;
    int headerIndex = 0;
    int readHeaderName = 0;
    int byteBody = 0;

    for (int i = 0; i < bytesRead; i++)
    {
        // controllo se ho trovato la fine della status line (CRLF CRLF)
        // prima cerco il \n, se lo trovo controllo se prima c'è un \r, se c'è allora ho trovato la fine della status line
        if (response[i] == '\n' && response[i - 1] == '\r')
        {
            if (response[i - 3] == 0)
            {
                body = response + i + 1; // trovato il body
                byteBody = bytesRead - (i + 1);
                break;
            }

            readHeaderName = 0;
            response[i - 1] = '\0';              // metto il terminatore di stringa al posto del '\r'
            h[headerIndex].n = response + i + 1; // salvato su h[0] il nome dell'header
        }
        else if (!readHeaderName && response[i] == ':')
        {
            readHeaderName = 1;
            response[i] = '\0';                    // metto il terminatore di stringa al posto del ':'
            h[headerIndex++].v = response + i + 1; // salvato su h[0] il valore dell'header
        }
    }

    char httpVersion[10];
    int statusCode;
    char statusPhrase[20];
    sscanf(statusLine, "%s %d %s", httpVersion, &statusCode, statusPhrase); // prima specifico dove leggere, poi la formattazione della stringa

    printf("status code: %d\n", statusCode);
    printf("number of headers read: %d\n", headerIndex);

    for (int i = 0; i < headerIndex - 1; i++)
    {
        printf("header %s = %s\n", h[i].n, h[i].v); // stampo tutti gli header letti
    }

    printf("responseLine: %s\n", statusLine);
    // printf("body: %s\n", body);
    printf("number of bytes in the body: %d\n", byteBody);

    int f = open("response.html", O_CREAT | O_WRONLY); // flag O_CREAT per creare il file se non esiste, flag O_WRONLY per aprire il file in scrittura
    write(f, body, byteBody);
    close(f);

    // printf("server says: %s\n", response);
}