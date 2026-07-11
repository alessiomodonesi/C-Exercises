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
    /* 
     * Definizione della struttura per memorizzare gli header HTTP.
     * Ciascun header ha un nome (n) e un valore (v).
     * Nota: l'ordine delle variabili nella struct è importante per l'allineamento.
     */
    struct header
    {
        char *n; // Nome dell'header (es. "Content-Length")
        char *v; // Valore dell'header (es. "1024")
    };

    struct header h[100]; // Array per memorizzare fino a 100 header
    char *statusLine;     // Conterrà la prima riga della risposta HTTP (Status Line)
    char *body;           // Conterrà il puntatore all'inizio del body della risposta

    // 1. Creazione del socket TCP (SOCK_STREAM) per IPv4 (AF_INET)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Configurazione dell'indirizzo del server di destinazione
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(80); // Porta 80 in Network Byte Order (Big Endian)

    // Impostazione manuale dell'indirizzo IP del server (es. 142.251.153.119) byte per byte
    char *ip = (char *)&address.sin_addr.s_addr;
    ip[0] = 142, ip[1] = 251, ip[2] = 153, ip[3] = 119;

    // 3. Connessione al server remoto
    int c = connect(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (c == 0)
        printf("connection established\n");
    else
    {
        perror("connection failed");
        exit(1);
    }

    // 4. Preparazione della richiesta HTTP/1.0 GET
    char buffer[] = "GET /imghp?hl=it&ogbl HTTP/1.0\r\n\r\n";

    // Invio della richiesta HTTP scrivendo nel socket finché tutti i byte non vengono inviati
    int m = 0;
    int writtenByte = 0;
    while (writtenByte < strlen(buffer))
    {
        m = write(sockfd, buffer + writtenByte, strlen(buffer) - writtenByte);
        writtenByte += m;
    }

    // 5. Lettura della risposta HTTP dal socket
    char response[1000000]; // Buffer capiente per contenere la risposta
    int n = 0;
    int readByte = 0;
    
    // Legge continuamente dal socket fino a quando la connessione non viene chiusa dal server (read ritorna 0)
    while ((n = read(sockfd, response + readByte, sizeof(response) - readByte)) > 0)
    {
        readByte += n;
    }

    // 6. Parsing della risposta HTTP (in-place modification del buffer)
    statusLine = response; // La status line inizia all'inizio del buffer
    int headerIndex = 0;
    int readHeaderName = 0; // Flag per capire se stiamo leggendo il nome dell'header o il valore
    int byteBody = 0;       // Dimensione del body in byte

    for (int i = 0; i < readByte; i++)
    {
        // Controllo se ho trovato la fine di una riga (sequenza CRLF ovvero \r\n)
        if (response[i] == '\n' && response[i - 1] == '\r')
        {
            /*
             * Riconoscimento della fine degli header (\r\n\r\n):
             * Quando finisce un header precedente, il carattere '\r' viene sostituito con '\0'.
             * Di conseguenza, se troviamo un \r\n in cui la posizione i-3 (corrispondente al \r 
             * della riga prima, ora sostituito) è 0, significa che abbiamo due sequenze CRLF consecutive.
             * Questo segna la fine degli header e l'inizio del body.
             */
            if (response[i - 3] == 0)
            {
                body = response + i + 1; // Il body inizia subito dopo il carattere '\n' corrente
                byteBody = readByte - (i + 1);
                break;
            }

            readHeaderName = 0; // Resetta il flag per la riga successiva (inizierà con il nome dell'header)
            response[i - 1] = '\0';              // Sostituisce il '\r' con il terminatore di stringa '\0'
            h[headerIndex].n = response + i + 1; // Salva il puntatore all'inizio del nome dell'header successivo
        }
        // Se non stiamo ancora leggendo il valore dell'header e troviamo il separatore ':'
        else if (!readHeaderName && response[i] == ':')
        {
            readHeaderName = 1; // Passiamo alla lettura del valore
            response[i] = '\0';                    // Sostituisce il ':' con il terminatore di stringa per separare il nome
            h[headerIndex++].v = response + i + 1; // Salva il puntatore al valore dell'header e incrementa l'indice
        }
    }

    // 7. Estrazione delle informazioni dalla Status Line (es: HTTP/1.0 200 OK)
    char httpVersion[10];
    int statusCode;
    char statusPhrase[20];
    // Legge la versione HTTP, lo status code e la frase di stato
    sscanf(statusLine, "%s %d %s", httpVersion, &statusCode, statusPhrase);

    // Stampa dei risultati del parsing a schermo
    printf("status code: %d\n", statusCode);
    printf("number of headers read: %d\n", headerIndex);

    // Stampa di tutti gli header estratti (tranne l'ultimo elemento parziale dell'array)
    for (int i = 0; i < headerIndex - 1; i++)
    {
        printf("header %s = %s\n", h[i].n, h[i].v);
    }

    printf("responseLine: %s\n", statusLine);
    printf("number of bytes in the body: %d\n", byteBody);

    // 8. Salvataggio del body (es: la pagina HTML) su file
    // O_CREAT: crea il file se non esiste. O_WRONLY: apre in sola scrittura. Permessi: 0666 (lettura e scrittura per tutti)
    int f = open("response.html", O_CREAT | O_WRONLY, 0666);
    write(f, body, byteBody);
    close(f);
}