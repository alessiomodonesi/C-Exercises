#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * Client HTTP/1.1 che gestisce risposte con codifica "Transfer-Encoding: chunked".
 */
int main()
{
    // Struttura per memorizzare gli header HTTP (nome e valore)
    struct header
    {            // ricorda: l'ordine della variabili è importante
        char *n; // header name
        char *v; // header value
    };

    struct header h[100];
    char *statusLine;
    char *body;

    // Creazione del socket TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(80); // Porta standard HTTP (80)

    // Impostazione dell'indirizzo IP del server (es. www.google.com)
    char *ip = (char *)&address.sin_addr;
    // ip[0] = 217, ip[1] = 61, ip[2] = 15, ip[3] = 222; // www.example.it
    // ip[0] = 172, ip[1] = 66, ip[2] = 147, ip[3] = 243; // www.example.com
    ip[0] = 142, ip[1] = 251, ip[2] = 29, ip[3] = 101; // www.google.com

    // Connessione al server
    int c = connect(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (c == 0)
    {
        printf("connessione stabilita\n");
    }
    else
    {
        perror("connessione fallita");
        exit(1);
    }

    // Invio della richiesta HTTP GET in versione 1.1
    // su Linux è possibile inviare una richiesta HTTP con telnet, basta scrivere "telnet www.example.com 80" e
    // poi scrivere la richiesta HTTP (ad esempio "GET / HTTP/1.1\r\nHost:www.example.com\r\nConnection:keep-alive\r\n\r\n")

    char buffer[] = "GET / HTTP/1.1\r\nHost:www.google.com\r\nConnection:keep-alive\r\n\r\n"; // ora in versione 1.1
    // Connection:close -> quando il server ha finito di inviare cose chiude la connessione (come nella 1.0)
    // Connection:keep-alive -> quando il server ha finito di inviare cose non chiude la connessione
    // ovviamente dopo l'inserimento di eventuali headers devo mettere CRLF CRLF (\r\n\r\n)

    // Gestione dell'invio completo della richiesta
    int bytesWritten = 0;
    int m = 0;
    while (bytesWritten < (int)strlen(buffer))
    {
        m = write(sockfd, buffer + bytesWritten, strlen(buffer) - bytesWritten);
        bytesWritten += m;
    }

    printf("ho scritto %d bytes\n", bytesWritten);

    // Buffer per la risposta (1MB)
    char response[1000000];

    int n = 0;
    int byteBody = 0;
    int headerIndex = 0;
    int headerNameFlag = 0;
    int bytesRead = 0;

    // Lettura della risposta byte per byte per il parsing degli header
    // lettura byte per byte con parsing interno
    while ((n = read(sockfd, response + bytesRead, 1)) > 0)
    {
        // Controllo della fine di una riga (CRLF)
        // controllo se ho trovato la fine della status line (CRLF CRLF)
        // prima cerco il \n, se lo trovo controllo se prima c'è un \r, se c'è allora ho trovato la fine della status line
        if (response[bytesRead] == '\n' && response[bytesRead - 1] == '\r')
        {
            // Se troviamo un doppio CRLF, gli header sono finiti
            if (response[bytesRead - 3] == 0)
            {
                body = response + bytesRead + 1; // trovato il body
                break;
            }

            headerNameFlag = 0;
            response[bytesRead - 1] = 0;                 // metto il terminatore di stringa al posto del '\r'
            h[headerIndex].n = response + bytesRead + 1; // salvato su h[0] il nome dell'header
        }
        // Parsing dell'header: ricerca del separatore ':'
        else if (!headerNameFlag && response[bytesRead] == ':')
        {
            headerNameFlag = 1;
            response[bytesRead] = 0;                       // metto il terminatore di stringa al posto del ':'
            h[headerIndex++].v = response + bytesRead + 1; // salvato su h[0] il valore dell'header
        }
        bytesRead += n;
    }

    printf("ho letto tutti gli header\n");
    statusLine = response;

    int statusCode;
    char httpVersion[10];
    char statusPhrase[20];

    // Estrazione dei dati dalla Status Line
    // prima specifico dove leggere, poi la formattazione della stringa
    sscanf(statusLine, "%s %d %s", httpVersion, &statusCode, statusPhrase);
    printf("status code = %d\n", statusCode);

    printf("numero di header letti = %d\n", headerIndex - 1);

    // Variabile per segnalare se la risposta è codificata a blocchi (chunked)
    int transferEncoding = 0;
    for (int i = 0; i < headerIndex; i++)
    {
        printf("header %s =%s\n", h[i].n, h[i].v); // stampo tutti gli header letti

        if (strcmp("Content-Length", h[i].n) == 0)
            sscanf(h[i].v, "%d", &byteBody);

        // Controllo se è presente l'header Transfer-Encoding: chunked
        if (strcmp("Transfer-Encoding", h[i].n) == 0 && strcmp(h[i].v, " chunked") == 0)
        {
            transferEncoding = 1;
            printf("Transfer-Encoding: chunked\n");
        }
    }

    printf("numero di caratteri del body = %d\n", byteBody);
    // printf("valore del body: \n%s", body);

    int chunkSize = 0;
    char *chunkValue = body;
    int chunkedFlag = 0;

    // Gestione della ricezione del body se codificato a blocchi (chunked)
    if (transferEncoding)
    {
        n = 0;
        // Leggo la risposta un byte alla volta per identificare i confini dei chunk
        while ((n += read(sockfd, body + n, 1)) > 0)
        {
            // I chunk sono delimitati da CRLF
            if (body[n - 1] == '\n' && body[n - 2] == '\r')
            {
                body[n - 2] = 0; // Temporaneamente chiudo la stringa per leggerla con sscanf

                if (chunkedFlag)
                {
                    // Abbiamo appena finito di leggere il contenuto di un chunk (CRLF finale del chunk)
                    chunkedFlag = 0;
                    n -= 2;                // Rimuovo i caratteri CRLF dal conteggio dei dati utili
                    chunkValue = body + n; // Il prossimo valore sarà la dimensione del prossimo chunk
                }
                else
                {
                    // Abbiamo appena letto la dimensione del prossimo chunk in esadecimale
                    sscanf(chunkValue, "%x", &chunkSize); // leggo il numero di byte del chunk in esadecimale
                    printf("valore del chunk = %d = 0x%x\n", chunkSize, chunkSize);

                    // Un chunk di dimensione 0 indica la fine della trasmissione
                    if (chunkSize == 0)
                        break;

                    fflush(stdout);                // svuoto il buffer dello std output
                    n -= (strlen(chunkValue) + 2); // Sovrascrivo la stringa della dimensione con i dati reali
                    chunkedFlag = 1;
                    byteBody += chunkSize; // Aggiorno la dimensione totale del body

                    // Leggo esattamente chunkSize byte di dati del chunk
                    int m = 0;
                    while ((m += read(sockfd, body + n + m, 1)) > 0 && m < chunkSize)
                    {
                    }
                    n += m;
                }
            }
        }
    }
    else
    {
        // Ricezione standard basata su Content-Length
        n = 0;
        while ((n += read(sockfd, body + n, 1)) > 0 && n < byteBody)
        {
        }
    }

    // Lettura di eventuali dati residui dopo l'ultimo chunk (trailer)
    char trailer[1000];
    n = 0;
    while ((n += read(sockfd, trailer + n, 1)) > 0)
    {
        if (trailer[n - 1] == '\n' && trailer[n - 2] == '\r')
        {
            break;
        }
    }
    printf("trailer size: %d\n", n);

    // Chiusura del socket
    close(sockfd);
    printf("numero di byte letti = %d\n", n);
    printf("responseLine = %s\n", statusLine);

    // Salvataggio del contenuto ricostruito in un file HTML
    // flag O_CREAT per creare il file se non esiste, aggiunto permessi 0644
    int f = open("response.html", O_CREAT | O_WRONLY, 0644);

    write(f, body, byteBody);
    close(f);

    return 0;
}
