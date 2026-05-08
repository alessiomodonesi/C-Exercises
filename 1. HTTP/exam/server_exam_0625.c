#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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
    // Il testo dice che la porta dovrebbe essere letta dal file "port.txt",
    // qui è hardcodata a 9012 (nota per un'eventuale correzione)
    address.sin_port = htons(9012);
    address.sin_addr.s_addr = INADDR_ANY;

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

    int l = listen(sockfd, 5);
    printf("il server è pronto per l'accept\n");
    fflush(stdout);

    while (1)
    {
        int clientSockId = accept(sockfd, NULL, NULL);
        int forkId = fork();

        if (forkId == 0)
        {
            printf("accept effettuata\n");
            char buffer[10000];
            printf("mi preparo a leggere\n");
            fflush(stdout);

            int n = 0;
            int lettoNomeHeader = 0;
            int byteLetti = 0;
            int headerIndex = 0;

            // Lettura e parsing degli header della richiesta HTTP
            while ((n += read(clientSockId, buffer + n, 1)) > 0)
            {
                if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
                {
                    if (buffer[n - 4] == 0)
                    {
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

            char *user_agent;
            int contentLength = 0;
            printf("stampo gli headers\n");

            // --- INIZIO IDENTIFICAZIONE CLIENT ---
            // Il server identifica il client in base alle informazioni della richiesta (User-Agent)
            for (int i = 0; i < headerIndex; i++)
            {
                if (strcmp(h[i].n, "User-Agent") == 0)
                {
                    user_agent = h[i].v; // Salva la stringa per capire se è Chrome, Curl o Firefox
                }
            }
            printf("userAgent = %s\n\n", user_agent);
            // --- FINE IDENTIFICAZIONE CLIENT ---

            char *requestLine = buffer;
            printf("request line:%s\n", requestLine);
            char method[10], uri[100], version[10];

            sscanf(requestLine, "%s %s %s", method, uri, version);
            printf("method:%s\nuri:%s\nversion:%s\n", method, uri, version);

            char response[1000] = "HTTP/1.1 200 OK\r\nTransfer-Encoding:chunked\r\n\r\n";
            char *user_agent_chrome = " Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/147.0.0.0 Safari/537.36";
            char *user_agent_curl = " curl";

            // ====================================================================
            // CASO 1: CLIENT È CHROME
            // Il server deve rifiutare la richiesta e rispondere con errore
            // ====================================================================
            if (strcmp(user_agent, user_agent_chrome) == 0)
            {
                printf("ho letto chrome\n\n");
                // Invia codice 406 Not Acceptable (Client-Error 4xx)
                sprintf(response, "HTTP/1.1 406 Not Acceptable\r\n\r\n");
                inviaByte(clientSockId, response, strlen(response));
                close(clientSockId);
            }
            else
            {
                if (strcmp(method, "GET") == 0)
                {
                    if (strcmp(uri, "/") == 0)
                    {
                        sprintf(uri, "/index.html");
                    }

                    int fd = open(uri + 1, O_RDONLY);
                    if (fd < 0)
                    {
                        inviaByte(clientSockId, response, strlen(response));
                        sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html>PAGINA NON TROVATA!</html>");
                    }
                    else
                    {

                        // ====================================================================
                        // CASO 3: CLIENT È CURL IN LOCALE
                        // Il server deve inviare codice 200 OK e rimuovere i tag HTML
                        // ====================================================================
                        if (memcmp(user_agent_curl, user_agent, 5) == 0)
                        {
                            // Risponde con 200 OK (senza chunked)
                            sprintf(response, "HTTP/1.1 200 OK\r\n\r\n");
                            inviaByte(clientSockId, response, strlen(response));

                            int m = 0;
                            char bufferFile[1024];
                            int tagAperturaLetto = 0; // Variabile di stato per filtrare i tag

                            // Legge il file 1 byte alla volta per trovare e scartare i tag <...>
                            while ((m = read(fd, bufferFile, 1)) > 0)
                            {
                                if (bufferFile[0] == '<')
                                {
                                    tagAperturaLetto = 1; // Inizia il tag, non lo invio
                                    continue;
                                }
                                else if (tagAperturaLetto == 1 && bufferFile[0] != '>')
                                {
                                    continue; // Sono dentro il tag, ignoro il contenuto
                                }
                                else if (bufferFile[0] == '>')
                                {
                                    tagAperturaLetto = 0; // Fine del tag
                                    continue;             // Non invio nemmeno il carattere '>'
                                }
                                // Invia al client solo il testo puro, senza delimitatori HTML
                                inviaByte(clientSockId, bufferFile, m);
                            }
                        }
                        // ====================================================================
                        // CASO 2: CLIENT È FIREFOX (O ALTRO)
                        // Il server risponde con 200 OK e invia il file completo di tag HTML
                        // ====================================================================
                        else
                        {
                            // Invia gli header (che contengono Transfer-Encoding: chunked)
                            inviaByte(clientSockId, response, strlen(response));
                            int m = 0;
                            char bufferFile[1024];

                            // Legge e invia l'intero contenuto normalmente in blocchi (chunk)
                            while ((m = read(fd, bufferFile, sizeof(bufferFile))) > 0)
                            {
                                char chunkSize[100];
                                sprintf(chunkSize, "%x\r\n", m);
                                printf("sto inviando m = %d, 0x%s byte ", m, chunkSize);
                                inviaByte(clientSockId, chunkSize, strlen(chunkSize));
                                inviaByte(clientSockId, bufferFile, m);
                                inviaByte(clientSockId, "\r\n", 2);
                            }
                            inviaByte(clientSockId, "0\r\n\r\n", 5);
                        }
                    }
                }
                else if (strcmp(method, "POST") == 0)
                {
                    int m = 0;
                    char bufferFile[1024];

                    while ((m += read(clientSockId, bufferFile + m, sizeof(bufferFile))) < contentLength)
                    {
                    }
                    printf("buffer Body:%s\n", bufferFile);
                    inviaByte(clientSockId, response, strlen(response));
                }
                else
                {
                    sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
                    inviaByte(clientSockId, response, strlen(response));
                }

                close(clientSockId);
                return 0;
            }
        }
        else
        {
            close(clientSockId);
        }
    }
}