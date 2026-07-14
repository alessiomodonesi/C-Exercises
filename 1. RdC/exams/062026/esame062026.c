#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

// IMPLEMENTAZIONE: Modificato il server per proteggere le risorse private contro attacchi CSRF:
// 1. Le risorse pubbliche (es. /index.html) continuano ad essere servite liberamente.
// 2. Per le risorse protette (es. /private.html), il server verifica l'header Referer rispetto a una lista di domini trusted.
// 3. Se il Referer manca o non è trusted, verifica l'header Sec-Fetch-Site autorizzando solo "same-origin" o "same-site".
// 4. In caso di fallimento dei controlli risponde con "403 Forbidden", mentre risponde con "404 Not Found" se la risorsa non esiste.

/*
 * Funzione ausiliaria per garantire l'invio completo di un buffer di byte.
 * La write() di sistema potrebbe scrivere meno byte di quelli richiesti (scrittura parziale).
 * Questa funzione cicla fino a quando tutti i 'numeroByte' sono stati effettivamente trasmessi.
 */
void inviaByte(int fd, char *buffer, int numeroByte)
{
    int m = 0;
    int byteScritti = 0;
    while (byteScritti < numeroByte)
    {
        m = write(fd, buffer + byteScritti, numeroByte - byteScritti);
        if (m < 0)
        {
            break;
        }
        byteScritti += m;
    }
}

int main()
{
    /*
     * Struttura per memorizzare ciascun header HTTP letto.
     * n punta al nome dell'header (es. "Referer"), v punta al valore (es. " http://partner1.com").
     */
    struct header
    {
        char *n;
        char *v;
    };

    struct header h[100]; // Array per memorizzare fino a 100 intestazioni HTTP

    // Creazione del socket TCP (AF_INET, SOCK_STREAM)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Opzione SO_REUSEADDR per riutilizzare la porta subito dopo lo spegnimento del server,
    // evitando l'errore "Address already in use".
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8088);       // Porta di ascolto del server: 8088
    address.sin_addr.s_addr = INADDR_ANY; // Accetta connessioni su qualunque interfaccia

    // Associazione del socket a indirizzo e porta
    int s = bind(sockfd, (struct sockaddr *)&address, sizeof(address));
    if (s != 0)
    {
        perror("Bind fallita\n");
        exit(1);
    }
    else
    {
        printf("Bind riuscita\n");
        fflush(stdout);
    }

    // Configurazione del socket per l'ascolto (massimo 5 client in coda)
    int l = listen(sockfd, 5);
    printf("Il server è pronto per l'accept\n");
    fflush(stdout);

    // Ciclo principale del server
    while (1)
    {
        // Accetta una nuova connessione
        int clientSockID = accept(sockfd, NULL, NULL);

        // Concorrenza: fork per delegare la gestione della richiesta a un processo figlio
        int forkId = fork();
        if (forkId == 0) // Processo figlio
        {
            printf("Accept effettuata\n");

            char buffer[100000];
            printf("Mi preparo a leggere\n");
            fflush(stdout);

            int n = 0;
            int indiceHeader = 0;
            int lettoNomeHeader = 0;

            /*
             * PARSING DEGLI HEADERS DELLA RICHIESTA HTTP:
             * Si legge la richiesta carattere per carattere.
             * Vengono sostituiti i caratteri ':' e '\r' con dei '\0' (terminatori di stringa)
             * in modo da isolare i nomi e i valori degli header all'interno di 'buffer'.
             *
             * NOTA SULLA SPAZIATURA DEI VALORI:
             * Il valore dell'header (h[indiceHeader].v) punta al carattere subito successivo al ':'.
             * Di conseguenza, se nella richiesta HTTP è presente uno spazio dopo il due punti (es. "Referer: partner1.com"),
             * la stringa in h[i].v inizierà con quello spazio (" partner1.com").
             */
            while ((n += read(clientSockID, buffer + n, 1)) > 0)
            {

                if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
                {
                    // RILEVAZIONE FINE HEADERS (RIGA VUOTA):
                    // Se buffer[n - 4] == 0, significa che siamo in presenza di due CRLF consecutivi (\r\n\r\n).
                    // Infatti, il terminatore \0 inserito al termine della riga precedente (n - 2 del ciclo precedente)
                    // si trova esattamente 4 byte prima della fine dell'attuale sequenza \r\n, indicando l'assenza di dati
                    // tra le due terminazioni di riga.
                    if (buffer[n - 4] == 0)
                    {
                        break; // Gli header sono finiti, segue il body (se presente)
                    }
                    lettoNomeHeader = 0;
                    buffer[n - 2] = 0;              // Termina la riga inserendo \0 al posto di \r
                    h[indiceHeader].n = buffer + n; // Il prossimo carattere sarà l'inizio del prossimo header
                }
                else if (!lettoNomeHeader && buffer[n - 1] == ':')
                {
                    buffer[n - 1] = 0; // Termina la stringa del nome dell'header al posto del ':'
                    lettoNomeHeader = 1;
                    h[indiceHeader++].v = buffer + n; // Salva l'inizio della stringa del valore dell'header
                }
            }

            printf("Stampo gli headers\n");
            int contentLength = 0;
            char *referer = NULL;
            char *secFetchSite = NULL;

            // Scansione degli header estratti per individuare quelli rilevanti per il controllo CSRF e POST
            for (int i = 0; i < indiceHeader; i++)
            {
                printf("%s: %s\n", h[i].n, h[i].v);
                if (strcmp(h[i].n, "Content-Length") == 0)
                {
                    sscanf(h[i].v, "%d", &contentLength);
                }

                // Referer (indica l'indirizzo della pagina web precedente da cui proviene la richiesta)
                if (strcmp(h[i].n, "Referer") == 0)
                {
                    referer = h[i].v;
                }
                // Sec-Fetch-Site (indica la relazione di provenienza impostata dal browser: same-origin, same-site, cross-site, none)
                else if (strcmp(h[i].n, "Sec-Fetch-Site") == 0)
                {
                    secFetchSite = h[i].v;
                }
            }
            printf("Content length = %d\n", contentLength);

            // La Request Line è la prima stringa all'inizio del buffer
            char *requestLine = buffer;
            printf("Request line: %s\n", requestLine);

            char method[10], uri[100], version[10];
            // Estrazione di metodo, URI e versione HTTP
            sscanf(requestLine, "%s %s %s", method, uri, version);
            printf("Method: %s, URI: %s, Version: %s\n", method, uri, version);

            // Risposta predefinita con codice HTTP 200 OK
            char response[1024] = "HTTP/1.1 200 OK\r\n\r\n";

            // Gestione del metodo GET
            if (strcmp(method, "GET") == 0)
            {

                if (strcmp(uri, "/") == 0)
                {
                    sprintf(uri, "/index.html");
                }

                // Apertura del file richiesto (uri + 1 per saltare lo slash '/' iniziale)
                int fd = open(uri + 1, O_RDONLY);
                if (fd < 0)
                {
                    // REQUISITO: Risorsa inesistente -> Risponde con 404 Not Found
                    sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html><body>Pagina non trovata</body></html>");
                    inviaByte(clientSockID, response, (int)strlen(response));
                }
                else
                {
                    int autorizzato = 0;

                    // REQUISITO: Controllo di accesso per risorse protette (es. "/private.html")
                    if (strcmp(uri, "/private.html") == 0)
                    {
                        /*
                         * LOGICA DI VALIDAZIONE CSRF (Requisito d'Esame):
                         *
                         * 1. Verifica del Referer:
                         * Se l'header Referer è presente e contiene "partner1.com" (che fa parte
                         * dei domini trusted della nostra lista), l'accesso è considerato autorizzato.
                         */
                        if (referer != NULL && strstr(referer, "partner1.com") != NULL)
                        {
                            autorizzato = 1;
                        }
                        /*
                         * 2. Verifica del Sec-Fetch-Site (Fallback):
                         * Qualora il Referer sia assente o non trusted, si esamina l'header Sec-Fetch-Site.
                         * Se Sec-Fetch-Site è presente e contiene "same-origin" oppure "same-site",
                         * la richiesta viene considerata autorizzata (poiché proviene dallo stesso dominio o sito).
                         * In tutti gli altri casi (cross-site, none o assenza dell'header), l'accesso viene rifiutato.
                         */
                        else if (secFetchSite != NULL)
                        {
                            if (strstr(secFetchSite, "same-origin") != NULL || strstr(secFetchSite, "same-site") != NULL)
                            {
                                autorizzato = 1;
                            }
                        }

                        // Se la richiesta non ha superato i controlli di validazione (autorizzato == 0),
                        // il server restituisce errore "403 Forbidden" ed interrompe la connessione.
                        if (autorizzato == 0)
                        {
                            sprintf(response, "HTTP/1.1 403 Forbidden\r\n\r\n<html><body>Accesso negato</body></html>");
                            inviaByte(clientSockID, response, (int)strlen(response));
                            close(fd);
                            close(clientSockID);
                            return 0;
                        }
                    }

                    // Se la risorsa è pubblica o se l'accesso alla risorsa protetta è stato autorizzato:
                    // Invia l'header 200 OK seguito dal corpo del file
                    inviaByte(clientSockID, response, (int)strlen(response));

                    int m = 0;
                    char bufferFile[1024];
                    // Lettura del file a blocchi di 1024 byte ed invio dei byte letti al client
                    while ((m = read(fd, bufferFile, sizeof(bufferFile))) > 0)
                    {
                        inviaByte(clientSockID, bufferFile, m);
                    }
                    close(fd);
                }
            }
            // Gestione opzionale del metodo POST
            else if (strcmp(method, "POST") == 0)
            {
                int m = 0;
                char bufferFile[10000] = {0};

                // Lettura controllata del corpo della richiesta POST in base a contentLength
                while (m < contentLength && m < (int)sizeof(bufferFile) - 1)
                {
                    int r = read(clientSockID, bufferFile + m, sizeof(bufferFile) - 1 - m);
                    if (r <= 0)
                    {
                        break;
                    }
                    m += r;
                }

                printf("Buffer body ricevuto:\n%s\n", bufferFile);
                sprintf(response, "HTTP/1.1 200 OK\r\n\r\n<html><body>Dati POST ricevuti correttamente</body></html>");
                inviaByte(clientSockID, response, (int)strlen(response));
            }
            // Gestione di metodi HTTP non supportati
            else
            {
                sprintf(response, "HTTP / 1.1 405 Method Not Allowed\r\n\r\n");
                inviaByte(clientSockID, response, (int)strlen(response));
            }

            close(clientSockID);
            return 0; // Termina il processo figlio
        }
        else // Processo padre
        {
            // Il padre chiude la propria copia del socket del client e si rimette in ascolto di nuove richieste
            close(clientSockID);
        }
    }
}