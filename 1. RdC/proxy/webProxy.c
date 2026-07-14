#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <netdb.h>

/*
 * Funzione helper per inviare un numero preciso di byte su un socket/descrittore.
 * Gestisce scritture parziali effettuando chiamate a write() in ciclo.
 */
int inviaByte(int fd, char *buffer, int numeroByte)
{
    int byteScritti = 0;
    int m = 0;
    while (byteScritti < numeroByte)
    {
        m = write(fd, buffer + byteScritti, numeroByte - byteScritti);
        byteScritti += m;
    }
    return byteScritti;
}

int main()
{
    /*
     * Struttura per contenere le coppie nome-valore degli header HTTP ricevuti dal client.
     */
    struct header
    {
        char *n;
        char *v;
    };

    struct header h[100]; // Array per memorizzare fino a 100 header

    // 1. Creazione del socket TCP del proxy (ascolto connessioni in ingresso dai client)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Abilita il riutilizzo dell'indirizzo locale (SO_REUSEADDR) per evitare l'errore "Address already in use" al riavvio
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configurazione dell'indirizzo su cui ascoltare (porta 9012, qualsiasi interfaccia di rete)
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(9012);
    address.sin_addr.s_addr = INADDR_ANY;

    // Associazione del socket all'indirizzo e alla porta
    int s = bind(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (s != 0)
    {
        perror("bind fallita");
        exit(1);
    }
    else
    {
        printf("bind avvento con successo");
        fflush(stdout);
    }

    // Messa in ascolto sul socket (coda massima di 5 connessioni in attesa)
    int l = listen(sockfd, 5);

    printf("il server è pronto per l'accept");
    fflush(stdout);

    // Loop principale per accettare e gestire le connessioni in ingresso
    while (1)
    {
        // Accetta una nuova connessione da un client
        int clientSockId = accept(sockfd, NULL, NULL);

        // Fork per gestire la connessione concorrentemente in un processo figlio
        int forkId = fork();
        if (forkId == 0) // Processo figlio
        {
            printf("accept effettuata");

            char buffer[10000]; // Buffer per la richiesta del client

            printf("mi preparo a leggere\n");
            fflush(stdout);

            int n = 0;
            int lettoNomeHeader = 0;
            int byteLetti = 0;
            int headerIndex = 0;

            /*
             * Lettura della richiesta del client byte per byte.
             * Viene effettuato anche il parsing in-place dei campi header.
             */
            while ((n += read(clientSockId, buffer + n, 1)) > 0)
            {
                printf("%c", buffer[n - 1]);

                // Controllo del ritorno a capo (\r\n) per identificare la fine di una riga
                if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
                {
                    /*
                     * Riconoscimento del doppio CRLF (\r\n\r\n) che indica la fine degli header.
                     * Il carattere a posizione n-4 sarà 0 se il \r della riga precedente è stato impostato a 0.
                     */
                    if (buffer[n - 4] == 0)
                    {
                        // body = buffer + byteLetti + 1;
                        break;
                    }
                    lettoNomeHeader = 0;           // Ripristina il flag per la riga successiva
                    buffer[n - 2] = 0;             // Sostituisce il '\r' con un terminatore '\0'
                    h[headerIndex].n = buffer + n; // Imposta il puntatore al nome dell'header successivo
                }
                // Rilevamento del separatore ':' per dividere nome e valore dell'header
                else if (!lettoNomeHeader && buffer[n - 1] == ':')
                {
                    lettoNomeHeader = 1;             // Stiamo per leggere il valore dell'header
                    buffer[n - 1] = 0;               // Sostituisce il ':' con un terminatore '\0'
                    h[headerIndex++].v = buffer + n; // Imposta il puntatore al valore e incrementa l'indice degli header
                }
            }

            char *contentLengthValue;
            int contentLength = 0;
            printf("stampo gli headers");

            // Scansione degli header estratti per individuare il Content-Length (necessario per le richieste POST)
            for (int i = 0; i < headerIndex; i++)
            {
                if (strcmp(h[i].n, "Content-Length") == 0)
                {
                    sscanf(h[i].v, "%d", &contentLength);
                    contentLengthValue = h[i].v + 1; // Salva il puntatore al valore saltando lo spazio iniziale
                }
                printf("%s:%s\n", h[i].n, h[i].v);
            }

            printf("content length = %d\n", contentLength);
            char *requestLine = buffer; // La Request Line è all'inizio del buffer
            printf("request line:%s", requestLine);

            char method[10], uri[100], version[10];

            // Estrazione di metodo, URI e versione HTTP dalla Request Line
            sscanf(requestLine, "%s %s %s", method, uri, version);

            printf("method:%s\n", method);
            printf("uri:%s\n", uri);
            printf("version:%s\n", version);

            if (strcmp(method, "CONNECT") == 0)
                printf("metodo = connect");
            else
                printf("metodo <> connect");

            fflush(stdout);

            char response[1000] = "HTTP/1.1 200 OK\r\nTransfer-Encoding:chunked\r\n\r\n";

            // --- CASO 1: GESTIONE DEL METODO HTTP GET ---
            // DIFFERENZA DAL SERVER CLASSICO:
            // Invece di cercare un file locale sul proprio disco fisso, il proxy interpreta
            // l'URI per estrarre l'hostname remoto, vi si connette via socket, inoltra la richiesta
            // e ridirige la risposta indietro al client.
            if (strcmp(method, "GET") == 0)
            {
                // Se l'URI è la root, imposta il file di default
                if (strcmp(uri, "/") == 0)
                {
                    sprintf(uri, "/index.html");
                }

                // [DIFFERENZA] Identificazione del separatore "://" nell'URI.
                // Un proxy riceve spesso URI assoluti (es. http://www.google.com/index.html)
                // mentre un web server classico riceve di norma solo percorsi relativi (es. /index.html).
                int j = 0;
                for (j; j < strlen(uri); j++)
                {
                    if (j > 0 && uri[j] == '/' && uri[j - 1] == '/') // identificato "://"
                        break;
                }
                char *hostname = uri + j + 1;
                char *new_uri;
                new_uri = uri + j;
                j++;

                // [DIFFERENZA] Parsing per separare l'hostname reale dal path della risorsa richiesta.
                // Es: "www.esempio.com/percorso/file.html" -> hostname: "www.esempio.com", new_uri: "percorso/file.html"
                for (j; j < strlen(uri); j++)
                {
                    if (j > 0 && uri[j] == '/')
                    {
                        uri[j] = 0;            // Termina la stringa dell'hostname
                        new_uri = uri + j + 1; // Il nuovo URI relativo inizia dopo la barra '/'

                        break;
                    }
                }

                printf("hostname = %s\n", hostname);
                printf("uri = %s\n", new_uri);

                // [DIFFERENZA] Creazione di un client socket (socket2).
                // Un server classico non effettua connessioni outbound (in uscita) verso altri server web.
                int socket2 = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in address2;

                address2.sin_family = AF_INET;
                address2.sin_port = htons(80); // Porta HTTP standard per il server di destinazione

                // [DIFFERENZA] Risoluzione DNS dell'hostname remoto.
                // Il server classico risponde solo sul proprio IP; il proxy deve risolvere l'IP di terzi.
                struct hostent *addr = gethostbyname(hostname);
                address2.sin_addr.s_addr = *(unsigned int *)addr->h_addr;

                // [DIFFERENZA] Connessione attiva (connect) verso il server remoto di destinazione.
                int c = connect(socket2, (struct sockaddr *)&address2, sizeof(address2));
                char request2[1000];

                // [DIFFERENZA] Riformulazione della richiesta HTTP/1.1 da inoltrare.
                // Viene specificato esplicitamente l'header "Host" con il target remoto.
                sprintf(request2, "GET /%s HTTP/1.1\r\nConnection:close\r\nHost:%s\r\n\r\n", new_uri, hostname);

                // [DIFFERENZA] Invio della richiesta appena ricostruita al server remoto.
                inviaByte(socket2, request2, strlen(request2));

                // [DIFFERENZA] Ponte di inoltro (Relay Loop):
                // Legge la risposta proveniente dal server remoto (socket2) e la scrive
                // direttamente sul socket del client originario (clientSockId) in tempo reale.
                char buffer2[1000];
                int m = 0;
                while ((m = read(socket2, buffer2, sizeof(buffer2))) > 0)
                {
                    inviaByte(clientSockId, buffer2, m);
                }
            }
            // --- CASO 2: GESTIONE DEL METODO HTTP POST ---
            else if (strcmp(method, "POST") == 0)
            {
                // Se l'URI inizia con "/cgi-bin", gestisce la richiesta eseguendo uno script esterno tramite CGI
                if (memcmp(uri, "/cgi-bin", 8) == 0)
                {
                    int pid = fork(); // Fork per eseguire lo script CGI in isolamento
                    if (pid == 0)     // Processo figlio CGI
                    {
                        char *queryString = NULL;

                        // Ricerca del carattere '?' nell'URI per estrarre la Query String
                        int i;
                        for (i = 0; uri[i] != 0 && uri[i] != '?'; i++)
                        {
                        }

                        if (uri[i] == '?')
                        {
                            uri[i] = 0; // Separa l'URI dallo script e dai parametri
                            printf("il valore della uri = %s\n\n", uri);
                            queryString = uri + i + 1;
                            printf("il valore della queryString = %s\n\n", queryString);
                            // Imposta la variabile d'ambiente QUERY_STRING per lo script CGI
                            setenv("QUERY_STRING", queryString, 1);
                        }

                        // Redirezione di standard input e standard output sul socket del client
                        dup2(clientSockId, 0); // stdin associato al socket (per leggere il corpo del POST)
                        dup2(clientSockId, 1); // stdout associato al socket (per scrivere la risposta direttamente al client)

                        // Impostazione delle variabili d'ambiente per il protocollo CGI
                        setenv("METHOD", "POST", 1);
                        setenv("ContentLength", contentLengthValue, 1);

                        // Scrittura dell'header di risposta HTTP standard prima di eseguire lo script
                        printf("HTTP/1.1 200 OK\r\n\r\n");

                        // Esecuzione dello script CGI specificato nell'URI (uri+1 rimuove la barra '/' iniziale)
                        execv(uri + 1, NULL);
                    }
                    else // Processo padre (gestore della connessione)
                    {
                        // Attende la terminazione dello script CGI figlio
                        waitpid(pid, NULL, 0);
                    }

                    // printf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html>PAGINA NON TROVATA!</html");
                    // inviaByte(clientSockId, response, strlen(response));
                }
                else // Se non è CGI, gestisce un POST standard leggendo il body della richiesta
                {
                    int m = 0;
                    char bufferFile[1024];

                    // Lettura ciclica del body fino a raggiungere il numero di byte specificato in Content-Length
                    while ((m += read(clientSockId, bufferFile + m, sizeof(bufferFile))) < contentLength)
                    {
                    }

                    printf("buffer Body:%s\n", bufferFile);
                    // Invio di una risposta di conferma standard (response inizializzata precedentemente)
                    inviaByte(clientSockId, response, strlen(response));
                }
            }
            // --- CASO 3: GESTIONE DEL METODO HTTP CONNECT (Tunneling SSL/TLS HTTPS) ---
            // DIFFERENZA DAL SERVER CLASSICO:
            // Questa è una funzionalità esclusiva dei Proxy Server (HTTP Tunneling).
            // Un server web classico non implementa CONNECT. Serve a stabilire una connessione TCP
            // cieca e bidirezionale diretta tra il client e un server sicuro (es. porta 443 HTTPS),
            // senza che il proxy possa o debba decifrare il traffico (TLS/SSL).
            else if (strcmp(method, "CONNECT") == 0)
            {
                printf("sono nella connect\n");
                fflush(stdout);
                char *port;
                int j;

                // [DIFFERENZA] Estrazione di indirizzo e porta dall'URI di destinazione (formato host:porta, es. google.com:443)
                for (j = 0; uri[j] != ':'; j++)
                {
                }

                uri[j] = 0;         // Termina la stringa dell'hostname
                port = uri + j + 1; // Punta alla stringa contenente la porta

                printf("address a cui connettersi:%s\n", uri);
                int portInt = atoi(port);
                printf("porta a cui connettersi:%d da stringa = %s\n", portInt, port);
                fflush(stdout);

                // [DIFFERENZA] Creazione del socket verso il server sicuro di destinazione.
                int socket2 = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in address2;

                address2.sin_family = AF_INET;
                address2.sin_port = htons(portInt); // Porta di destinazione finale (solitamente 443)

                // [DIFFERENZA] Risoluzione DNS dell'host di destinazione sicura.
                struct hostent *addr = gethostbyname(uri);
                address2.sin_addr.s_addr = *(unsigned int *)addr->h_addr;

                // [DIFFERENZA] Connessione TCP al server remoto sicuro.
                int c = connect(socket2, (struct sockaddr *)&address2, sizeof(address2));

                // [DIFFERENZA] Il proxy invia al client lo stato "HTTP/1.1 200 Established".
                // Questo comunica al client che il canale TCP fisico è stato aperto con successo
                // e che da questo momento in poi può iniziare a trasmettere pacchetti SSL/TLS cifrati.
                char buffer2[1000];
                sprintf(buffer2, "HTTP/1.1 200 Established\r\n\r\n");
                inviaByte(clientSockId, buffer2, strlen(buffer2));

                // [DIFFERENZA] Fork per sdoppiare il flusso di rete e permettere una comunicazione Full-Duplex.
                // Il proxy non interpreta più i dati come HTTP, ma si limita a fare un relay a livello di trasporto (TCP).
                int fork2 = fork();

                if (fork2 == 0) // Processo figlio del tunnel: inoltro da Destinazione Remota a Client
                {
                    char bufferClient[1000];
                    int m = 0;
                    while ((m = read(socket2, bufferClient, sizeof(bufferClient))) > 0)
                    {
                        inviaByte(clientSockId, bufferClient, m);
                    }
                }
                else // Processo padre del tunnel: inoltro da Client a Destinazione Remota
                {
                    char bufferClient[1000];
                    int m = 0;
                    while ((m = read(clientSockId, bufferClient, sizeof(bufferClient))) > 0)
                    {
                        inviaByte(socket2, bufferClient, m);
                    }
                }
            }
            // --- CASO 4: METODO NON SUPPORTATO ---
            else
            {
                sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
                inviaByte(clientSockId, response, strlen(response));
            }

            // Chiusura del socket di comunicazione con il client nel processo figlio ed uscita
            close(clientSockId);
            return 0;
        }
        else // Processo padre (server principale)
        {
            // Il processo padre chiude la sua copia del socket client e torna in ascolto sull'accept
            close(clientSockId);
        }
    }
}