#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// VISTO DAGLI APPUNTI

/*
 * Funzione di utilità per garantire l'invio completo di un buffer di byte su un socket.
 * La chiamata di sistema write() standard potrebbe non trasmettere tutti i byte richiesti
 * in una sola volta (scrittura parziale). Questa funzione esegue un ciclo finché
 * tutti i 'numeroByte' specificati non sono stati inviati con successo.
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
     * Definizione di una struttura per memorizzare gli header HTTP estratti.
     * Ciascun header è memorizzato come coppia chiave/valore (nome 'n' e valore 'v').
     */
    struct header
    {
        char *n;
        char *v;
    };

    struct header h[100]; // Array per salvare fino a 100 header HTTP della richiesta

    // Creazione del socket TCP (SOCK_STREAM) su protocollo IPv4 (AF_INET)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Configurazione delle opzioni del socket: SO_REUSEADDR permette di riutilizzare
    // immediatamente la porta locale per il bind anche se il server viene riavviato di frequente,
    // evitando l'errore "Address already in use".
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;

    /*
     * NOTA DI DISCREPANZA CON IL TESTO D'ESAME:
     * Il testo dell'appello specifica che il numero di porta sul quale mettere il server
     * in ascolto deve essere letto dal file "port.txt" situato nella directory home del cloud.
     * In questo codice la porta è invece hardcodata a 9012.
     * Per conformarsi al testo, bisognerebbe aprire e leggere il file "port.txt" a runtime.
     */
    address.sin_port = htons(9012);
    address.sin_addr.s_addr = INADDR_ANY; // Accetta connessioni da qualsiasi interfaccia di rete

    // Associazione dell'indirizzo e della porta al socket
    int s = bind(sockfd, (struct sockaddr *)&address, sizeof(address));

    if (s != 0)
    {
        perror("bind fallita");
        exit(1);
    }
    else
    {
        printf("bind avvento con successo\n");
        fflush(stdout);
    }

    // Configurazione del socket in ascolto, con una coda massima di 5 connessioni in attesa
    int l = listen(sockfd, 5);
    printf("il server è pronto per l'accept\n");
    fflush(stdout);

    // Loop infinito del server per accettare e gestire le richieste dei client
    while (1)
    {
        // Accetta una nuova connessione in ingresso dal client
        int clientSockId = accept(sockfd, NULL, NULL);

        // Creazione di un processo figlio tramite fork per gestire la richiesta in modo concorrente
        int forkId = fork();

        if (forkId == 0) // Processo figlio
        {
            printf("accept effettuata\n");
            char buffer[10000];
            printf("mi preparo a leggere\n");
            fflush(stdout);

            int n = 0;
            int lettoNomeHeader = 0;
            int byteLetti = 0;
            int headerIndex = 0;

            /*
             * LETTURA E PARSING DEGLI HEADER DELLA RICHIESTA HTTP:
             * Si legge la richiesta dal socket 1 byte alla volta.
             *
             * Il parser memorizza i puntatori all'interno di 'buffer' inserendo dei caratteri nulli '\0'
             * al posto dei separatori di riga (\r\n) e del carattere due punti (':').
             *
             * PARTICOLARITÀ DEL PARSING:
             * Poiché il valore 'v' dell'header viene fatto puntare subito dopo il carattere ':',
             * il valore includerà l'eventuale spazio iniziale inviato dal client (es. "User-Agent: curl"
             * produrrà h[i].n = "User-Agent" e h[i].v = " curl" con spazio in testa).
             */
            while ((n += read(clientSockId, buffer + n, 1)) > 0)
            {
                // Riconoscimento della fine di una riga HTTP (\r\n)
                if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
                {
                    // RILEVAZIONE FINE DEGLI HEADERS:
                    // Se buffer[n - 4] == 0, significa che abbiamo letto una riga vuota (\r\n\r\n).
                    // Infatti, il terminatore \0 inserito al termine della riga precedente (in buffer[n-2]
                    // del ciclo precedente) si trova esattamente 4 byte prima del '\n' corrente,
                    // a indicare che non vi sono caratteri in questa riga prima di \r\n.
                    if (buffer[n - 4] == 0)
                    {
                        break; // Fine della sezione header
                    }
                    lettoNomeHeader = 0;
                    buffer[n - 2] = 0;             // Sostituisce '\r' con il terminatore di stringa '\0'
                    h[headerIndex].n = buffer + n; // Il prossimo carattere sarà l'inizio del prossimo header
                }
                // Rilevazione del carattere ':' che separa il nome dell'header dal valore
                else if (!lettoNomeHeader && buffer[n - 1] == ':')
                {
                    lettoNomeHeader = 1;
                    buffer[n - 1] = 0;               // Sostituisce ':' con il terminatore di stringa '\0'
                    h[headerIndex++].v = buffer + n; // Salva il puntatore al valore dell'header
                }
            }

            char *user_agent;
            int contentLength = 0;
            printf("stampo gli headers\n");

            /*
             * IDENTIFICAZIONE DEL CLIENT (Requisito d'Esame):
             * Scansione degli header per individuare il valore di "User-Agent",
             * fondamentale per determinare la tipologia di client.
             */
            for (int i = 0; i < headerIndex; i++)
            {
                if (strcmp(h[i].n, "User-Agent") == 0)
                {
                    user_agent = h[i].v; // Salva il valore dello User-Agent
                }
            }
            printf("userAgent = %s\n\n", user_agent);

            // La Request Line (es: "GET /index.html HTTP/1.1") si trova all'inizio del buffer
            char *requestLine = buffer;
            printf("request line:%s\n", requestLine);
            char method[10], uri[100], version[10];

            // Parsing di metodo, URI e versione HTTP
            sscanf(requestLine, "%s %s %s", method, uri, version);
            printf("method:%s\nuri:%s\nversion:%s\n", method, uri, version);

            // Risposta predefinita con codifica Chunked (utilizzata per Firefox)
            char response[1000] = "HTTP/1.1 200 OK\r\nTransfer-Encoding:chunked\r\n\r\n";

            // Definizione degli User-Agent attesi.
            // Si noti lo spazio iniziale dovuto al parsing del valore dell'header.
            char *user_agent_chrome = " Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/147.0.0.0 Safari/537.36";
            char *user_agent_curl = " curl";

            /*
             * REQUISITO 1: CLIENT CHROME (REMOTO)
             * Se la richiesta proviene da Chrome (User-Agent corrispondente), il server deve rifiutare
             * la richiesta rispondendo con un errore HTTP appropriato (RFC 9110, Section 15, errore 4xx).
             * Qui viene usato il codice "406 Not Acceptable".
             */
            if (strcmp(user_agent, user_agent_chrome) == 0)
            {
                printf("ho letto chrome\n\n");
                sprintf(response, "HTTP/1.1 406 Not Acceptable\r\n\r\n");
                inviaByte(clientSockId, response, strlen(response));
                close(clientSockId);
            }
            else
            {
                // Gestione della richiesta HTTP di tipo GET
                if (strcmp(method, "GET") == 0)
                {
                    // Reindirizzamento della root "/" a "/index.html"
                    if (strcmp(uri, "/") == 0)
                    {
                        sprintf(uri, "/index.html");
                    }

                    /*
                     * NOTA DI DISCREPANZA CON IL TESTO D'ESAME:
                     * Le specifiche d'esame indicano: "Il file da restituire è sempre lo stesso prova.html".
                     * Il codice attuale apre invece il file specificato nell'URI (saltando il primo carattere '/', ossia uri + 1).
                     * Per rispettare rigorosamente il testo, si dovrebbe forzare l'apertura di "prova.html".
                     */
                    int fd = open(uri + 1, O_RDONLY);
                    if (fd < 0)
                    {
                        // Gestione errore 404 File Not Found
                        inviaByte(clientSockId, response, strlen(response));
                        sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html>PAGINA NON TROVATA!</html>");
                    }
                    else
                    {
                        /*
                         * REQUISITO 3: CLIENT CURL (LOCALE)
                         * Se la richiesta proviene da curl (verificato confrontando i primi 5 caratteri con " curl"),
                         * il server risponde con 200 OK ma rimuove completamente tutti i tag HTML (cioè tutto ciò che
                         * si trova tra '<' e '>').
                         *
                         * Logica di rimozione tag (State Machine):
                         * - Viene letto il file 1 byte alla volta.
                         * - Se leggiamo '<' -> tagAperturaLetto = 1 (entriamo nel tag, scartiamo il carattere).
                         * - Se siamo dentro al tag e leggiamo qualcosa diverso da '>' -> continuiamo a scartare.
                         * - Se leggiamo '>' -> tagAperturaLetto = 0 (usciamo dal tag, scartiamo il carattere).
                         * - Se siamo fuori dal tag (tagAperturaLetto == 0) -> il carattere viene inviato al client.
                         */
                        if (memcmp(user_agent_curl, user_agent, 5) == 0)
                        {
                            // Risponde con 200 OK normale (non chunked, inviando direttamente il testo puro)
                            sprintf(response, "HTTP/1.1 200 OK\r\n\r\n");
                            inviaByte(clientSockId, response, strlen(response));

                            int m = 0;
                            char bufferFile[1024];
                            int tagAperturaLetto = 0; // Stato del parser: 0 = testo, 1 = dentro tag HTML

                            // Lettura a singolo carattere per un filtraggio preciso dei tag
                            while ((m = read(fd, bufferFile, 1)) > 0)
                            {
                                if (bufferFile[0] == '<')
                                {
                                    tagAperturaLetto = 1; // Inizio del tag HTML
                                    continue;             // salta alla prossima iterazione del while
                                }
                                else if (tagAperturaLetto == 1 && bufferFile[0] != '>')
                                {
                                    continue; // Ignora il contenuto interno del tag
                                }
                                else if (bufferFile[0] == '>')
                                {
                                    tagAperturaLetto = 0; // Fine del tag HTML
                                    continue;
                                }
                                // Invia il carattere di testo puro al client
                                inviaByte(clientSockId, bufferFile, m);
                            }
                        }
                        /*
                         * REQUISITO 2: CLIENT MOZILLA FIREFOX (O ALTRI)
                         * Il server deve rispondere con codice 200 OK e fornire l'intero file HTML richiesto,
                         * completo di tutti i tag e formattazione HTML.
                         *
                         * In questo caso, il server invia il file tramite Transfer-Encoding: chunked.
                         * Ogni pezzo di file letto viene preceduto dalla sua dimensione in esadecimale seguita da \r\n,
                         * e terminato con un blocco vuoto "0\r\n\r\n".
                         */
                        else
                        {
                            // Invia gli header di risposta (Transfer-Encoding: chunked)
                            inviaByte(clientSockId, response, strlen(response));
                            int m = 0;
                            char bufferFile[1024];

                            // Legge e invia il file a blocchi di 1024 byte
                            while ((m = read(fd, bufferFile, sizeof(bufferFile))) > 0)
                            {
                                char chunkSize[100];
                                // Formatta la dimensione del chunk in esadecimale
                                sprintf(chunkSize, "%x\r\n", m);
                                printf("sto inviando m = %d, 0x%s byte ", m, chunkSize);
                                inviaByte(clientSockId, chunkSize, strlen(chunkSize)); // Invia dimensione chunk
                                inviaByte(clientSockId, bufferFile, m);                // Invia dati chunk
                                inviaByte(clientSockId, "\r\n", 2);                    // Fine blocco chunk
                            }
                            // Invia il chunk finale di terminazione (dimensione zero)
                            inviaByte(clientSockId, "0\r\n\r\n", 5);
                        }
                    }
                }
                // Gestione opzionale del metodo POST
                else if (strcmp(method, "POST") == 0)
                {
                    int m = 0;
                    char bufferFile[1024];

                    // Legge il corpo della richiesta fino al completamento dei byte previsti da contentLength
                    while ((m += read(clientSockId, bufferFile + m, sizeof(bufferFile))) < contentLength)
                    {
                    }
                    printf("buffer Body:%s\n", bufferFile);
                    inviaByte(clientSockId, response, strlen(response));
                }
                // Gestione di altri metodi HTTP non supportati
                else
                {
                    sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
                    inviaByte(clientSockId, response, strlen(response));
                }

                close(clientSockId);
                return 0; // Termina il processo figlio per questa connessione
            }
        }
        else // Processo padre
        {
            // Il padre chiude la propria copia del socket del client e torna in ascolto di nuove connessioni
            close(clientSockId);
        }
    }
}
