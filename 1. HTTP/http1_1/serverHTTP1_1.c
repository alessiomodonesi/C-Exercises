#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void sendByte(int fd, char *buffer, int numeroByte)
{
    int m = 0;
    int byteScritti = 0;
    while (byteScritti < numeroByte)
    {
        m = write(fd, buffer + byteScritti, numeroByte - byteScritti);
        if (m < 0)
            break; // Evita loop infiniti in caso di errore (Aggiunta di sicurezza)
        byteScritti += m;
    }
}

int main()
{
    struct header
    {
        char *n;
        char *v;
    };

    struct header h[100];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8081);
    address.sin_addr.s_addr = INADDR_ANY;

    int s = bind(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (s != 0)
    {
        perror("bind fallita\n");
        exit(1);
    }
    else
    {
        printf("bind avvenuta con successo\n");
        fflush(stdout);
    }

    int l = listen(sockfd, 5); // posso accettare 5 client in parallelo

    printf("il server è pronto per l'accept\n");
    fflush(stdout);

    while (1)
    {
        int clientSockId = accept(sockfd, NULL, NULL);

        int forkId = fork(); // creo un processo figlio per gestire la richiesta del client, così il processo padre può continuare ad accettare altre richieste
        if (forkId == 0)     // entro nel processo figlio, gestisco la richiesta del client
        {
            printf("accept effettuata\n");

            // Buffer aumentato per poter leggere interamente richieste con body corposi (es. POST)
            char buffer[10000];

            printf("mi preparo a leggere\n");
            fflush(stdout);

            int n = 0;
            int lettoNomeHeader = 0;
            // int byteLetti = 0;
            int headerIndex = 0;

            // Ciclo per leggere la richiesta carattere per carattere fino alla fine degli header (\r\n\r\n)
            while ((n += read(clientSockId, buffer + n, 1)) > 0)
            {
                if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
                {
                    if (buffer[n - 4] == 0)
                    {
                        // Raggiunta la riga vuota che separa header e body (\r\n\r\n)
                        break;
                    }
                    lettoNomeHeader = 0;
                    buffer[n - 2] = 0;
                    h[headerIndex].n = buffer + n;
                }
                else if (!lettoNomeHeader && buffer[n - 1] == ':')
                {
                    lettoNomeHeader = 1;
                    buffer[n - 1] = 0;
                    h[headerIndex++].v = buffer + n;
                }
            }

            // Variabile per salvare la dimensione del body (fondamentale per le richieste POST)
            int contentLength = 0;
            printf("stampo gli headers\n");
            for (int i = 0; i < headerIndex; i++)
            {
                // Cerco l'header Content-Length e ne estraggo il valore intero
                if (strcmp(h[i].n, "Content-Length") == 0)
                {
                    sscanf(h[i].v, "%d", &contentLength);
                }
                printf("%s:%s\n", h[i].n, h[i].v);
            }
            printf("content length = %d\n", contentLength);

            char *requestLine = buffer; // Es. estrazione di: GET /index.html HTTP/1.1
            printf("request line: %s\n", requestLine);

            char method[10], uri[100], version[10];
            sscanf(requestLine, "%s %s %s", method, uri, version);

            // I metodi supportati ora sono GET e POST. Gli altri (PUT, DELETE, ecc.) restituiranno 405 Method Not Allowed
            printf("method: %s\n", method);
            printf("uri: %s\n", uri);
            printf("version: %s\n", version);

            char response[1024];

            if (strcmp(method, "GET") == 0) // Gestione del metodo GET
            {
                // Prepara la risposta con Transfer-Encoding: chunked per inviare il file a blocchi
                strcpy(response, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

                if (strcmp(uri, "/") == 0)
                    sprintf(uri, "/index.html");

                int fd = open(uri + 1, O_RDONLY); // uri + 1 per saltare la barra iniziale (es. "/index.html" -> "index.html")

                if (fd < 0)
                {
                    // Se il file non esiste, preparo e invio l'errore 404
                    sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html><body>Pagina non trovata!</body></html>");
                    sendByte(clientSockId, response, strlen(response));
                }
                else
                {
                    // Chiamo la funzione definita sopra per inviare gli header HTTP al client
                    sendByte(clientSockId, response, strlen(response));

                    int m = 0;
                    char bufferFile[1024];

                    // Leggo il file e lo invio a blocchi (chunk) secondo lo standard HTTP/1.1
                    while ((m = read(fd, bufferFile, sizeof(bufferFile))) > 0)
                    {
                        char chunkSize[100];
                        sprintf(chunkSize, "%x\r\n", m); // Scrivo la dimensione del chunk in esadecimale seguita da \r\n
                        printf("sto inviando m = %d, 0x%s byte ", m, chunkSize);

                        sendByte(clientSockId, chunkSize, strlen(chunkSize)); // Invio dimensione chunk
                        sendByte(clientSockId, bufferFile, m);                // Invio contenuto chunk
                        sendByte(clientSockId, "\r\n", 2);                    // Invio terminatore chunk
                    }
                    // Invio il chunk vuoto di dimensione 0 per indicare al client la fine della trasmissione
                    sendByte(clientSockId, "0\r\n\r\n", 5);
                }

                close(fd);
            }
            else if (strcmp(method, "POST") == 0) // Gestione del metodo POST
            {
                int m = 0;
                char bufferFile[10000] = {0};

                // Leggo il body della richiesta iterando finché non ho letto tutti i byte indicati dal Content-Length
                while (m < contentLength && m < sizeof(bufferFile) - 1)
                {
                    int r = read(clientSockId, bufferFile + m, sizeof(bufferFile) - 1 - m);
                    if (r <= 0)
                        break;
                    m += r;
                }

                printf("buffer Body:\n%s\n", bufferFile);

                sprintf(response, "HTTP/1.1 200 OK\r\n\r\n<html><body>Dati POST ricevuti correttamente!</body></html>");
                // Chiamo la funzione definita sopra per inviare la risposta al client
                sendByte(clientSockId, response, strlen(response));
            }
            else // Se il metodo non è né GET né POST (es. DELETE, PUT)
            {
                sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
                // Chiamo la funzione definita sopra per inviare la risposta di errore al client
                sendByte(clientSockId, response, strlen(response));
            }

            close(clientSockId);
            return 0; // Termina il processo figlio
        }
        else
        {
            close(clientSockId); // Nel processo padre, chiudo il socket dedicato al client e mi rimetto in attesa sulla listen
        }
    }
}