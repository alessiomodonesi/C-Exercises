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
 * Client HTTP/1.1 semplice che effettua una richiesta GET e salva la risposta.
 */
int main()
{
    // Struttura per memorizzare gli header HTTP ricevuti (nome e valore)
    struct header
    {            // ricorda: l'ordine della variabili è importante
        char *n; // header name
        char *v; // header value
    };

    struct header h[100];
    char *statusLine;
    char *body;

    // Creazione del socket: AF_INET (IPv4), SOCK_STREAM (TCP), 0 (protocollo di default)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;

    // Configurazione dei dettagli del server a cui connettersi
    address.sin_family = AF_INET;
    address.sin_port = htons(80); // Connessione sulla porta standard HTTP (80)

    // Impostazione manuale dell'indirizzo IP di www.example.it (217.61.15.222)
    char *ip = (char *)&address.sin_addr;
    ip[0] = 217, ip[1] = 61, ip[2] = 15, ip[3] = 222; // www.example.it

    // Apertura della connessione verso il server
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

    // Preparazione della richiesta HTTP/1.1
    // su Linux è possibile inviare una richiesta HTTP con telnet, basta scrivere "telnet www.example.com 80" e
    // poi scrivere la richiesta HTTP (ad esempio "GET / HTTP/1.1\r\nHost:www.example.com\r\nConnection:keep-alive\r\n\r\n")

    char buffer[] = "GET / HTTP/1.1\r\nHost:www.example.it\r\nConnection:keep-alive\r\n\r\n"; // ora in versione 1.1
    // Connection:close -> quando il server ha finito di inviare cose chiude la connessione (come nella 1.0)
    // Connection:keep-alive -> quando il server ha finito di inviare cose non chiude la connessione
    // ovviamente dopo l'inserimento di eventuali headers devo mettere CRLF CRLF (\r\n\r\n)

    // Invio della richiesta HTTP al server gestendo invii parziali
    int byteScritti = 0;
    int m = 0;
    while (byteScritti < strlen(buffer))
    {
        m = write(sockfd, buffer + byteScritti, strlen(buffer) - byteScritti);
        byteScritti += m;
    }

    printf("ho scritto %d bytes\n", byteScritti);

    // Buffer di memoria per ospitare la risposta del server
    char response[1000000];

    int n = 0;
    int byteBody = 0;
    int headerIndex = 0;
    int lettoNomeHeader = 0;
    int byteLetti = 0;

    // Lettura della risposta byte per byte per eseguire il parsing degli header "al volo"
    // lettura byte per byte con parsing interno
    while ((n = read(sockfd, response + byteLetti, 1)) > 0)
    {
        // Ricerca del terminatore di riga CRLF (\r\n)
        // controllo se ho trovato la fine della status line (CRLF CRLF)
        // prima cerco il \n, se lo trovo controllo se prima c'è un \r, se c'è allora ho trovato la fine della status line
        if (response[byteLetti] == '\n' && response[byteLetti - 1] == '\r')
        {
            // Se troviamo un doppio CRLF, gli header sono finiti e inizia il body
            if (response[byteLetti - 3] == 0)
            {
                body = response + byteLetti + 1; // trovato il body
                break;
            }
            // Fine di un header: mettiamo uno 0 binario per "chiudere" la stringa del valore
            lettoNomeHeader = 0;
            response[byteLetti - 1] = 0;                 // metto il terminatore di stringa al posto del '\r'
            h[headerIndex].n = response + byteLetti + 1; // il prossimo header inizierà subito dopo
        }
        // Parsing dell'header: ricerca del separatore ':' tra nome e valore
        else if (!lettoNomeHeader && response[byteLetti] == ':')
        {
            lettoNomeHeader = 1;
            response[byteLetti] = 0;                       // metto il terminatore di stringa al posto del ':'
            h[headerIndex++].v = response + byteLetti + 1; // salvo il puntatore all'inizio del valore
        }

        byteLetti += n;
    }

    printf("ho letto tutti gli header\n");
    statusLine = response; // La prima riga letta è la Status Line

    // Analisi della Status Line (Versione HTTP, Codice di Stato, Messaggio)
    int statusCode;
    char httpVersion[10];
    char statusPhrase[20];
    sscanf(statusLine, "%s %d %s", httpVersion, &statusCode, statusPhrase); // prima specifico dove leggere, poi la formattazione della stringa
    printf("lo status code = %d\n", statusCode);

    printf("numero di header letti = %d\n", headerIndex - 1);

    // Scorrimento degli header per estrarre la lunghezza del body (Content-Length)
    for (int i = 0; i < headerIndex - 1; i++)
    {
        printf("header %s = %s\n", h[i].n, h[i].v); // stampo tutti gli header letti
        if (strcmp("Content-Length", h[i].n) == 0)
        {
            sscanf(h[i].v, "%d", &byteBody);
        }
    }

    printf("il numero di caratteri del body = %d\n", byteBody);
    // printf("valore del body:\n%s", body);

    // Lettura del corpo della risposta dal socket in base alla lunghezza indicata
    n = 0;
    while ((n += read(sockfd, body + n, 1)) > 0 && n < byteBody)
    {
    }

    // Chiusura del socket una volta terminata la lettura
    close(sockfd);
    printf("numero di byte letti = %d\n", n);
    printf("la responseLine=%s\n", statusLine);

    // Apertura di un file per salvare il body della risposta (index.html o simile)
    int f = open("response.html", O_CREAT | O_WRONLY, 0644); // flag O_CREAT per creare il file se non esiste, aggiunto permessi 0644

    // Scrittura dei dati nel file
    write(f, body, byteBody);
    close(f); // È buona norma chiudere i file descriptor
    // printf("il server dice:%s\n", response);

    return 0; // Successo
}
