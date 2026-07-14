#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

// IMPLEMENTAZIONE: Esteso il forward proxy didattico per funzionare come Reverse Proxy:
// 1. Il proxy ascolta sulla porta statica indicata nel file port.txt [cite: 34] ed esamina l'header "Host" delle richieste in arrivo.
// 2. Utilizza una tabella di mapping statica (array di struct) per instradare le richieste ai rispettivi backend:
//    - "www.sito1.com" viene inoltrato al backend locale 127.0.0.1:8888.
//    - "www.sito2.com" viene inoltrato al backend locale 127.0.0.1:8889.
// 3. Se l'host non è presente nella tabella di mapping, il proxy risponde direttamente al client con "502 Bad Gateway".

/*
 * Funzione ausiliaria per garantire l'invio completo di un buffer su un socket.
 * Gestisce eventuali scritture parziali effettuando un ciclo finché tutti
 * i byte richiesti ('numeroByte') non sono stati inviati.
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
}

int main()
{
    /*
     * REQUISITO D'ESAME: Tabella statica di mapping
     * Struttura per mappare i domini virtuali dei client sui relativi port di backend.
     */
    struct sitePort
    {
        char *sito;
        short int port;
    };

    // Tabella statica di mapping (Host -> Port di backend)
    struct sitePort mappa[2];

    mappa[0].sito = "www.sito1.com";
    mappa[0].port = 8888; // Backend per il sito 1 su 127.0.0.1:8888

    mappa[1].sito = "www.sito2.com";
    mappa[1].port = 8889; // Backend per il sito 2 su 127.0.0.1:8889

    /*
     * Struttura di appoggio per memorizzare temporaneamente gli header HTTP estratti.
     */
    struct header
    {
        char *n;
        char *v;
    };

    struct header h[100]; // Array per memorizzare fino a 100 intestazioni HTTP

    // Creazione del socket principale TCP (AF_INET, SOCK_STREAM)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Opzione SO_REUSEADDR per liberare immediatamente la porta in caso di riavvio del server
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;

    /*
     * NOTA SULLA PORTA DEL PROXY (Requisito d'Esame):
     * Il testo d'esame specifica di inserire nel codice la porta letta a mano dal file "port.txt".
     * In questa implementazione è stata impostata staticamente la porta 9012.
     */
    address.sin_port = htons(9012);
    address.sin_addr.s_addr = INADDR_ANY; // Accetta connessioni da qualsiasi client

    // Associazione del socket a indirizzo e porta
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

    // Messa in ascolto del socket con coda di massimo 5 connessioni
    int l = listen(sockfd, 5);

    printf("il server è pronto per l'accept");
    fflush(stdout);

    // Loop principale del server proxy
    while (1)
    {
        // Accetta una nuova richiesta di connessione
        int clientSockId = accept(sockfd, NULL, NULL);

        // Processo concorrente tramite fork
        int forkId = fork();
        if (forkId == 0) // Processo figlio
        {
            printf("accept effettuata");

            char buffer[10000];

            printf("mi preparo a leggere\n");
            fflush(stdout);
            int n = 0;
            int lettoNomeHeader = 1; // Inizializzato a 1 per non dividere la Request Line (che non contiene il carattere ':')
            int byteLetti = 0;
            int headerIndex = 0;

            /*
             * PARSING DEGLI HEADERS DELLA RICHIESTA HTTP:
             * Legge la richiesta dal socket client byte per byte.
             *
             * I separatori ':' e '\r' vengono sostituiti con '\0' per isolare le stringhe
             * corrispondenti a nomi e valori degli header all'interno di 'buffer'.
             */
            while ((n += read(clientSockId, buffer + n, 1)) > 0)
            {
                // printf("%c", buffer[n - 1]);
                if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
                {
                    // Rilevazione fine degli header HTTP (\r\n\r\n) tramite il terminatore precedente
                    if (buffer[n - 4] == 0)
                    {
                        break;
                    }
                    lettoNomeHeader = 0;           // Dalla riga successiva (che sarà un header) cerchiamo il carattere ':'
                    buffer[n - 2] = 0;             // Sostituisce '\r' con '\0'
                    h[headerIndex].n = buffer + n; // Imposta il puntatore al nome del prossimo header
                }
                else if (!lettoNomeHeader && buffer[n - 1] == ':')
                {
                    lettoNomeHeader = 1;
                    buffer[n - 1] = 0;               // Sostituisce ':' con '\0' per separare il nome del campo dal valore
                    h[headerIndex++].v = buffer + n; // Salva l'inizio del valore dell'header
                }
            }
            char *headerHost;
            char *contentLengthValue;
            int contentLength = 0;
            printf("stampo gli headers");

            // Scansione degli header estratti per cercare "Content-Length" e "Host"
            for (int i = 0; i < headerIndex; i++)
            {
                if (strcmp(h[i].n, "Content-Length") == 0)
                {
                    sscanf(h[i].v, "%d", &contentLength);

                    /*
                     * NOTA SULLA GESTIONE DEGLI SPAZI:
                     * Poiché h[i].v punta subito dopo il carattere ':', contiene uno spazio iniziale
                     * (es. " Content-Length: 12" -> " 12"). Per questo si usa "+ 1" per saltare lo spazio.
                     */
                    contentLengthValue = h[i].v + 1;
                }
                if (strcmp(h[i].n, "Host") == 0)
                {
                    // Salva il valore dell'header Host saltando lo spazio iniziale
                    headerHost = h[i].v + 1;
                }
            }

            /*
             * RIMOZIONE DELLA PORTA DALL'HEADER HOST:
             * Se l'header Host contiene una porta (es. "www.sito1.com:9012"),
             * questa viene rimossa sostituendo il carattere ':' con un terminatore '\0',
             * così da isolare solo il nome del dominio (es. "www.sito1.com").
             */
            for (int i = 0; i < strlen(headerHost); i++)
            {
                if (headerHost[i] == ':')
                    headerHost[i] = 0;
            }
            printf("\nheaderHost:%s\n\n", headerHost);

            // La prima riga del buffer contiene la Request Line
            char *requestLine = buffer;
            printf("request line:%s", requestLine);

            char method[10], uri[100], version[10];
            // Estrazione di metodo, URI (relativo nel caso di reverse proxy) e versione
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

            // Gestione del metodo GET
            if (strcmp(method, "GET") == 0)
            {
                if (strcmp(uri, "/") == 0)
                {
                    // sprintf(uri, "/index.html");
                }

                // Creazione del socket verso il server di backend
                int socket2 = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in address2;
                short int portaBackend = 0;

                // REQUISITO D'ESAME: Identificazione del backend basandosi sull'header Host
                for (int i = 0; i < 2; i++)
                {
                    if (strcmp(mappa[i].sito, headerHost) == 0)
                    {
                        portaBackend = mappa[i].port; // Trova la porta corrispondente al sito (8888 o 8889)
                    }
                }

                // REQUISITO D'ESAME: Se l'host richiesto non è in tabella, risponde con 502 Bad Gateway
                if (portaBackend == 0)
                {
                    char responseBadGateway[1000];
                    sprintf(responseBadGateway, "HTTP/1.1 502 Bad Gateway\r\n\r\n");
                    inviaByte(clientSockId, responseBadGateway, strlen(responseBadGateway));
                    exit(0); // Termina il processo figlio
                }

                // Configurazione dell'indirizzo di connessione del backend
                address2.sin_family = AF_INET;
                address2.sin_port = htons(portaBackend);

                printf("\n\nmi connetto alla porta:%d\n\n", portaBackend);

                /*
                 * IMPOSTAZIONE MANUALE DELL'IP DI BACKEND (127.0.0.1):
                 * Poiché i backend girano localmente, l'indirizzo IP viene forzato a 127.0.0.1.
                 * Viene fatto effettuando il cast a puntatore a char dell'indirizzo IPv4 ed
                 * impostando singolarmente i 4 byte dell'IP.
                 */
                char *ip = (char *)&address2.sin_addr.s_addr;
                ip[0] = 127;
                ip[1] = 0;
                ip[2] = 0;
                ip[3] = 1;

                // Connessione fisica al server di backend
                int c = connect(socket2, (struct sockaddr *)&address2, sizeof(address2));

                // Preparazione e invio della richiesta HTTP riformattata al backend
                char request2[1000];
                sprintf(request2, "GET %s HTTP/1.1\r\nConnection:close\r\nHost:%s\r\n\r\n", uri, headerHost);
                printf("\n\nsto inviando:%s\n\n", request2);
                inviaByte(socket2, request2, strlen(request2));

                char buffer2[1000];
                int m = 0;

                /*
                 * NOTA E ATTENZIONE (INNEFFICIENZA/BUG NEL CODICE ORIGINALE):
                 * La condizione di lettura contiene una valutazione logica:
                 * `read(socket2, buffer2, sizeof(buffer2) > 0)`
                 * A causa della precedenza degli operatori in C, l'espressione logica `sizeof(buffer2) > 0`
                 * viene valutata per prima, restituendo 1 (vero).
                 * Questo significa che il terzo parametro della read diventa staticamente 1,
                 * portando a leggere la risposta dal backend 1 solo byte alla volta!
                 * Poiché è richiesto di NON modificare il codice, questa struttura è stata mantenuta così com'era.
                 */
                while (m = read(socket2, buffer2, sizeof(buffer2) > 0))
                {
                    printf("risposta:%s\n", buffer2);
                    inviaByte(clientSockId, buffer2, m); // Invia la risposta letta dal backend direttamente al client
                }
            }
            // Gestione del metodo POST
            else if (strcmp(method, "POST") == 0)
            {
                // Supporto per Common Gateway Interface (CGI-bin) per l'esecuzione di script esterni
                if (memcmp(uri, "/cgi-bin", 8) == 0)
                {
                    int pid = fork();
                    if (pid == 0) // Processo nipote per eseguire lo script CGI
                    {
                        char *queryString = NULL;

                        int i;
                        // Cerca se sono presenti parametri di query string (delimitati da '?')
                        for (i = 0; uri[i] != 0 && uri[i] != '?'; i++)
                        {
                        }

                        if (uri[i] == '?')
                        {
                            uri[i] = 0;
                            printf("il valore della uri = %s\n\n", uri);
                            queryString = uri + i + 1;
                            printf("il valore della queryString = %s\n\n", queryString);
                            setenv("QUERY_STRING", queryString, 1); // Imposta la variabile d'ambiente QUERY_STRING
                        }

                        // Redirezione di standard input e standard output sul socket del client
                        dup2(clientSockId, 0); // stdin
                        dup2(clientSockId, 1); // stdout;
                        setenv("METHOD", "POST", 1);
                        setenv("ContentLength", contentLengthValue, 1);
                        printf("HTTP/1.1 200 OK\r\n\r\n");
                        execv(uri + 1, NULL); // Esegue il programma/script CGI
                    }
                    else
                    {
                        int status;
                        waitpid(pid, &status, 0); // Attesa del completamento del processo figlio CGI
                    }
                }
                else
                {
                    // Gestione di una richiesta POST standard: lettura del body
                    int m = 0;
                    char bufferFile[1024];

                    while ((m += read(clientSockId, bufferFile + m, sizeof(bufferFile))) < contentLength)
                    {
                    }
                    printf("buffer Body:%s\n", bufferFile);
                    inviaByte(clientSockId, response, strlen(response));
                }
            }
            // Gestione del metodo CONNECT (utilizzato per HTTPS tunneling nel proxy originario)
            else if (strcmp(method, "CONNECT") == 0)
            {
                printf("sono nella connect\n");
                fflush(stdout);
                char *port;
                int j;
                // Estrae la porta dall'URI (es: "www.google.com:443")
                for (j = 0; uri[j] != ':'; j++)
                {
                }

                uri[j] = 0;
                port = uri + j + 1;

                printf("address a cui connettersi:%s\n", uri);
                int portInt = atoi(port);
                printf("porta a cui connettersi:%d da stringa = %s\n", portInt, port);
                fflush(stdout);

                int socket2 = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in address2;

                address2.sin_family = AF_INET;
                address2.sin_port = htons(portInt);

                // Risoluzione DNS del nome dell'host remoto
                struct hostent *addr = gethostbyname(uri);
                address2.sin_addr.s_addr = *(unsigned int *)addr->h_addr;

                // Connessione verso l'host di destinazione
                int c = connect(socket2, (struct sockaddr *)&address2, sizeof(address2));

                char buffer2[1000];
                // Comunica al client che il tunnel è stato stabilito
                sprintf(buffer2, "HTTP/1.1 200 Established\r\n\r\n");
                inviaByte(clientSockId, buffer2, strlen(buffer2));

                int fork2 = fork();

                // Tunneling bidirezionale del traffico crittografato
                if (fork2 == 0)
                {
                    char bufferClient[1000];
                    int m = 0;
                    // Legge dal server di destinazione e invia al client
                    while ((m = read(socket2, bufferClient, sizeof(bufferClient))) > 0)
                    {

                        inviaByte(clientSockId, bufferClient, m);
                    }
                }
                else
                {
                    char bufferClient[1000];
                    int m = 0;
                    // Legge dal client e invia al server di destinazione
                    while ((m = read(clientSockId, bufferClient, sizeof(bufferClient))) > 0)
                    {
                        inviaByte(socket2, bufferClient, m);
                    }
                }
            }
            else
            {
                // Gestione dei metodi HTTP non supportati
                sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
                inviaByte(clientSockId, response, strlen(response));
            }

            close(clientSockId);
            return 0; // Termina il processo figlio
        }
        else // Processo padre
        {
            // Il padre chiude il socket e torna in attesa
            close(clientSockId);
        }
    }
}
