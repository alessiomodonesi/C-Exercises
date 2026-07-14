#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>

int main()
{
    /*
     * Struttura per memorizzare le coppie nome-valore degli header HTTP.
     * n: puntatore alla stringa col nome dell'header
     * v: puntatore alla stringa col valore dell'header
     */
    struct header // ricorda: l'ordine della variabili è importante
    {
        char *n; // header name
        char *v; // header value
    };

    struct header h[100]; // Array per memorizzare fino a 100 header
    char *statusLine;     // Conterrà la prima riga della risposta (status line)
    char *body;           // Conterrà il puntatore all'inizio del body HTTP

    // Creazione di un socket TCP/IP (IPv4)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(80); // Porta 80 in Network Byte Order (Big Endian)

    char *ip = (char *)&address.sin_addr.s_addr;
    // ip[0] = 142, ip[1] = 251, ip[2] = 153, ip[3] = 119;

    // gethostbyname() restituisce un puntatore a una struct hostent che contiene informazioni sull'host, tra cui l'indirizzo IP
    struct hostent *addr = gethostbyname("www.google.it");

    // h_addr_list è un array di puntatori a struct in_addr, prendo il primo elemento
    address.sin_addr.s_addr = *(unsigned int *)addr->h_addr;

    // Connessione al server risolto tramite gethostbyname
    int c = connect(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (c == 0)
        printf("connection established\n");
    else
    {
        perror("connection failed");
        exit(1);
    }

    // Richiesta HTTP/1.0 GET per la root '/'
    char buffer[] = "GET / HTTP/1.0\r\n\r\n";

    int m = 0;
    int writtenByte = 0;
    // Ciclo di scrittura per garantire l'invio di tutti i byte della richiesta
    while (writtenByte < strlen(buffer))
    {
        m = write(sockfd, buffer + writtenByte, strlen(buffer) - writtenByte);
        writtenByte += m;
    }

    printf("written %d bytes\n", writtenByte);

    char response[1000000]; // Buffer per contenere la risposta del server

    int n = 0;
    int readByte = 0;
    // Ciclo di lettura dal socket fino al termine della connessione da parte del server
    while ((n = read(sockfd, response + readByte, sizeof(response) - readByte)) > 0)
    {
        // printf("read %d bytes\n", n);
        readByte += n;
        // printf("total bytes read: %d\n", readByte);
    }

    statusLine = response; // La status line comincia all'inizio del buffer
    int headerIndex = 0;
    int readHeaderName = 0; // Flag: 0 = stiamo leggendo il nome, 1 = stiamo leggendo il valore
    int byteBody = 0;       // Dimensione del body in byte

    // Parsing degli header e della status line (in-place)
    for (int i = 0; i < readByte; i++)
    {
        // controllo se ho trovato la fine della status line (CRLF CRLF)
        // prima cerco il \n, se lo trovo controllo se prima c'è un \r, se c'è allora ho trovato la fine della status line
        if (response[i] == '\n' && response[i - 1] == '\r')
        {
            /*
             * Riconoscimento della fine degli header (\r\n\r\n):
             * Quando finisce un header precedente, il carattere '\r' viene sostituito con '\0'.
             * Se troviamo un \r\n in cui il carattere i-3 è 0, significa che c'era una riga vuota
             * (cioè \r\n immediatamente successivo ad un altro \r\n). Questa riga vuota separa
             * gli header dal body.
             */
            if (response[i - 3] == 0)
            {
                body = response + i + 1; // trovato il body
                byteBody = readByte - (i + 1);
                break;
            }

            readHeaderName = 0;                  // Ripristina il flag per l'header successivo
            response[i - 1] = '\0';              // metto il terminatore di stringa al posto del '\r'
            h[headerIndex].n = response + i + 1; // salvato su h[0] il nome dell'header
        }
        else if (!readHeaderName && response[i] == ':')
        {
            readHeaderName = 1;                    // Passa alla lettura del valore dell'header
            response[i] = '\0';                    // metto il terminatore di stringa al posto del ':'
            h[headerIndex++].v = response + i + 1; // salvato su h[0] il valore dell'header
        }
    }

    char httpVersion[10];
    int statusCode;
    char statusPhrase[20];
    // Estrazione delle informazioni dalla status line
    sscanf(statusLine, "%s %d %s", httpVersion, &statusCode, statusPhrase); // prima specifico dove leggere, poi la formattazione della stringa

    printf("status code: %d\n", statusCode);
    printf("number of headers read: %d\n", headerIndex);

    // Stampa di tutti gli header estratti
    for (int i = 0; i < headerIndex - 1; i++)
    {
        printf("header %s = %s\n", h[i].n, h[i].v); // stampo tutti gli header letti
    }

    printf("number of bytes in the body: %d\n", byteBody);
    // printf("body: %s\n", body);
    printf("responseLine: %s\n", statusLine);

    // Salvataggio del body della risposta in un file html locale
    int f = open("response.html", O_CREAT | O_WRONLY, 0666); // flag O_CREAT per creare il file se non esiste, flag O_WRONLY per aprire il file in scrittura
    write(f, body, byteBody);
    close(f);

    // printf("server says: %s\n", response);
}