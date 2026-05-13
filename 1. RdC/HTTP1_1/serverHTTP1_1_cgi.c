#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h> // Aggiunta per far funzionare waitpid() nella CGI

/**
 * Funzione di utilità per garantire l'invio di tutti i byte richiesti sul socket.
 */
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
    // Struttura per il parsing degli header HTTP
    struct header
    {
        char *n;
        char *v;
    };

    struct header h[100];

    // Creazione del socket TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Configurazione per il riutilizzo della porta
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8081);
    address.sin_addr.s_addr = INADDR_ANY;

    // Associazione socket-indirizzo
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

    // Server in ascolto (max 5 connessioni in coda)
    int l = listen(sockfd, 5); // posso accettare 5 client in parallelo

    printf("il server è pronto per l'accept\n");
    fflush(stdout);

    // Loop principale del server
    while (1)
    {
        int clientSockId = accept(sockfd, NULL, NULL);

        // Fork per gestire la richiesta in un processo separato
        int forkId = fork(); // creo un processo figlio per gestire la richiesta del client, così il processo padre può continuare ad accettare altre richieste
        if (forkId == 0)     // entro nel processo figlio, gestisco la richiesta del client
        {
            printf("accept effettuata\n");

            // Buffer per la lettura della richiesta
            // Buffer aumentato per poter leggere interamente richieste con body corposi (es. POST)
            char buffer[10000];

            printf("mi preparo a leggere\n");
            fflush(stdout);

            int n = 0;
            int lettoNomeHeader = 0;
            int headerIndex = 0;

            // Parsing degli header HTTP terminati da \r\n\r\n
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

            // Estrazione del Content-Length (necessario per POST e CGI)
            // Variabili per salvare la dimensione del body (fondamentale per POST e CGI)
            int contentLength = 0;
            char *contentLengthValue = NULL;

            printf("stampo gli headers\n");
            for (int i = 0; i < headerIndex; i++)
            {
                // Cerco l'header Content-Length e ne estraggo il valore
                if (strcmp(h[i].n, "Content-Length") == 0)
                {
                    sscanf(h[i].v, "%d", &contentLength);
                    contentLengthValue = h[i].v + 1; // +1 per saltare lo spazio e catturare il valore come stringa (per la CGI)
                }
                printf("%s:%s\n", h[i].n, h[i].v);
            }
            printf("content length = %d\n", contentLength);

            // Parsing della riga di richiesta (Metodo URI Versione)
            char *requestLine = buffer; // Es. estrazione di: GET /index.html HTTP/1.1
            printf("request line: %s\n", requestLine);

            char method[10], uri[100], version[10];
            sscanf(requestLine, "%s %s %s", method, uri, version);

            // I metodi supportati ora sono GET e POST. Gli altri (PUT, DELETE, ecc.) restituiranno 405 Method Not Allowed
            printf("method: %s\n", method);
            printf("uri: %s\n", uri);
            printf("version: %s\n", version);

            char response[1024];

            // --- GESTIONE METODO GET ---
            if (strcmp(method, "GET") == 0) // Gestione del metodo GET
            {
                if (strcmp(uri, "/") == 0)
                    sprintf(uri, "/index.html");

                // --- INIZIO BLOCCO CGI ---
                // Se l'URI inizia con /cgi-bin, eseguiamo lo script corrispondente
                if (memcmp(uri, "/cgi-bin", 8) == 0)
                {
                    int pid = fork(); // Nuova fork per eseguire lo script CGI
                    if (pid == 0)
                    {
                        char *queryString = NULL;
                        int i;
                        // Cerco la presenza di parametri nell'URL dopo il '?' (es. /cgi-bin/test?name=val)
                        // Cerco la presenza di parametri nell'URL dopo il '?'
                        for (i = 0; uri[i] != 0 && uri[i] != '?'; i++)
                        {
                        }

                        if (uri[i] == '?')
                        {
                            uri[i] = 0; // Separo il percorso dello script dai parametri
                            printf("il valore della uri = %s\n\n", uri);
                            queryString = uri + i + 1; // I parametri seguono il '?'
                            printf("il valore della queryString = %s\n\n", queryString);
                            // Passo i parametri allo script tramite la variabile d'ambiente standard CGI
                            setenv("QUERY_STRING", queryString, 1);
                        }

                        // Reindirizzamento dell'I/O dello script verso il socket del client
                        dup2(clientSockId, 0); // Dirotta stdin sul socket client (lo script legge dal client)
                        dup2(clientSockId, 1); // Dirotta stdout sul socket client (lo script scrive al client)
                        setenv("METHOD", "GET", 1);

                        // Invio l'intestazione HTTP iniziale prima di passare il controllo allo script
                        printf("HTTP/1.1 200 OK\r\n\r\n"); // Header generato dal server prima di passare la palla allo script

                        // Esecuzione dello script CGI
                        execv(uri + 1, NULL); // uri+1 perché open/execv vogliono il path senza la '/' iniziale
                        exit(1);              // Esce in caso di fallimento della execv
                    }
                    else
                    {
                        // Il server aspetta che lo script termini prima di chiudere la connessione
                        waitpid(pid, NULL, 0); // Il processo padre aspetta la fine dello script CGI
                    }
                }
                // --- FINE BLOCCO CGI ---
                else // Gestione normale dei file statici
                {
                    // Prepara la risposta con Transfer-Encoding: chunked per inviare il file a blocchi
                    strcpy(response, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

                    int fd = open(uri + 1, O_RDONLY); // uri + 1 per saltare la barra iniziale (es. "/index.html" -> "index.html")

                    if (fd < 0)
                    {
                        // Se il file non esiste, preparo e invio l'errore 404
                        sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html><body>Pagina non trovata!</body></html>");
                        sendByte(clientSockId, response, (int)strlen(response));
                    }
                    else
                    {
                        // Chiamo la funzione definita sopra per inviare gli header HTTP al client
                        sendByte(clientSockId, response, (int)strlen(response));

                        int m = 0;
                        char bufferFile[1024];

                        // Leggo il file e lo invio a blocchi (chunk) secondo lo standard HTTP/1.1
                        while ((m = read(fd, bufferFile, sizeof(bufferFile))) > 0)
                        {
                            char chunkSize[100];
                            sprintf(chunkSize, "%x\r\n", m); // Scrivo la dimensione del chunk in esadecimale seguita da \r\n
                            printf("sto inviando m = %d, 0x%s byte ", m, chunkSize);

                            sendByte(clientSockId, chunkSize, (int)strlen(chunkSize)); // Invio dimensione chunk
                            sendByte(clientSockId, bufferFile, m);                     // Invio contenuto chunk
                            sendByte(clientSockId, "\r\n", 2);                         // Invio terminatore chunk
                        }
                        // Invio il chunk vuoto di dimensione 0 per indicare al client la fine della trasmissione
                        sendByte(clientSockId, "0\r\n\r\n", 5);
                    }
                    close(fd);
                }
            }
            // --- GESTIONE METODO POST ---
            else if (strcmp(method, "POST") == 0) // Gestione del metodo POST
            {
                // --- INIZIO BLOCCO CGI ---
                // Gestione script CGI anche per richieste POST
                if (memcmp(uri, "/cgi-bin", 8) == 0)
                {
                    int pid = fork();
                    if (pid == 0)
                    {
                        char *queryString = NULL;
                        int i;
                        for (i = 0; uri[i] != 0 && uri[i] != '?'; i++)
                        {
                        }

                        if (uri[i] == '?')
                        {
                            uri[i] = 0;
                            printf("il valore della uri = %s\n\n", uri);
                            queryString = uri + i + 1;
                            printf("il valore della queryString = %s\n\n", queryString);
                            setenv("QUERY_STRING", queryString, 1);
                        }

                        // Reindirizzamento standard I/O sul socket
                        dup2(clientSockId, 0); // Dirotta stdin
                        dup2(clientSockId, 1); // Dirotta stdout
                        setenv("METHOD", "POST", 1);

                        // Per il POST è fondamentale comunicare la lunghezza del body allo script
                        if (contentLengthValue != NULL)
                        {
                            setenv("ContentLength", contentLengthValue, 1);
                        }

                        printf("HTTP/1.1 200 OK\r\n\r\n");
                        execv(uri + 1, NULL);
                        exit(1);
                    }
                    else
                    {
                        waitpid(pid, NULL, 0);
                    }
                }
                // --- FINE BLOCCO CGI ---
                else // Gestione standard dei dati in POST senza script CGI
                {
                    int m = 0;
                    char bufferFile[10000] = {0};

                    // Leggo il body della richiesta iterando finché non ho letto tutti i byte indicati dal Content-Length
                    while (m < contentLength && m < (int)sizeof(bufferFile) - 1)
                    {
                        int r = read(clientSockId, bufferFile + m, sizeof(bufferFile) - 1 - m);
                        if (r <= 0)
                            break;
                        m += r;
                    }

                    printf("buffer Body:\n%s\n", bufferFile);

                    sprintf(response, "HTTP/1.1 200 OK\r\n\r\n<html><body>Dati POST ricevuti correttamente!</body></html>");
                    // Chiamo la funzione definita sopra per inviare la risposta al client
                    sendByte(clientSockId, response, (int)strlen(response));
                }
            }
            else // Se il metodo non è né GET né POST (es. DELETE, PUT)
            {
                sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
                // Chiamo la funzione definita sopra per inviare la risposta di errore al client
                sendByte(clientSockId, response, (int)strlen(response));
            }

            // Chiusura del socket nel processo figlio
            close(clientSockId);
            return 0; // Termina il processo figlio
        }
        else
        {
            // Il processo padre chiude il socket dedicato e torna ad accettare nuovi client
            close(clientSockId); // Nel processo padre, chiudo il socket dedicato al client e mi rimetto in attesa sulla listen
        }
    }
}
