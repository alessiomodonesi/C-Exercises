#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// SVOLTO DA SOLO

/**
 * Funzione di utilità per garantire l'invio di tutti i byte richiesti.
 * Gestisce eventuali invii parziali da parte della system call write().
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
	// Struttura per memorizzare gli header della richiesta
	struct header {
		char *n;
		char *v;
	};

	// Struct per tenere traccia dello stato globale
	struct ServerState {
		int last_index;
		int user_access[1000]; // array dove l'indice corrisponde all'ID utente
	};

	struct header h[100];
	struct ServerState state;
	state.last_index = 0;
	memset(state.user_access, 0, sizeof(state.user_access));

	// Creazione del socket del server (TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Configurazione del socket per permettere il riutilizzo immediato dell'indirizzo/porta dopo la chiusura
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);       // Il server ascolta sulla porta 8080
	address.sin_addr.s_addr = INADDR_ANY; // Accetta connessioni da qualsiasi interfaccia di rete

	// Associazione del socket all'indirizzo e alla porta configurati
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

	// Messa in ascolto del socket: coda di massimo 5 connessioni pendenti
	int l = listen(sockfd, 5); // posso accettare 5 client in parallelo

	printf("il server è pronto per l'accept\n");
	fflush(stdout);

	// Loop principale per accettare le connessioni dei client
	while (1)
	{
		int clientSockId = accept(sockfd, NULL, NULL);

		printf("accept effettuata\n");

		// Buffer per leggere la richiesta HTTP
		// Buffer aumentato per poter leggere interamente richieste con body corposi (es. POST)
		char buffer[10000];

		fflush(stdout);

		int n = 0;
		int lettoNomeHeader = 0;
		// int byteLetti = 0;
		int headerIndex = 0;

		// Parsing della richiesta byte per byte fino alla fine degli header (\r\n\r\n)
		// Ciclo per leggere la richiesta carattere per carattere fino alla fine degli header (\r\n\r\n)
		while ((n += read(clientSockId, buffer + n, 1)) > 0)
		{
			// Fine di una riga negli header
			if (buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
			{
				// Controllo se abbiamo trovato la riga vuota (\r\n\r\n) che separa header e body
				if (buffer[n - 4] == 0)
				{
					// Raggiunta la riga vuota che separa header e body (\r\n\r\n)
					break;
				}
				lettoNomeHeader = 0;
				buffer[n - 2] = 0;             // Termino la stringa del valore dell'header precedente
				h[headerIndex].n = buffer + n; // Inizio potenziale del prossimo nome header
			}
			// Identificazione del separatore ':' tra nome e valore dell'header
			else if (!lettoNomeHeader && buffer[n - 1] == ':')
			{
				lettoNomeHeader = 1;
				buffer[n - 1] = 0;               // Termino la stringa del nome dell'header
				h[headerIndex++].v = buffer + n; // Inizio del valore dell'header
			}
		}

		// Ricerca del Content-Length per gestire eventuali body (es. in POST)
		// Variabile per salvare la dimensione del body (fondamentale per le richieste POST)
		int contentLength = 0;
		int new_user = 1;
		int user_index = 0;
		for (int i = 0; i < headerIndex; i++)
		{
			// Cerco l'header Content-Length e ne estraggo il valore intero
			if (strcmp(h[i].n, "Content-Length") == 0)
				sscanf(h[i].v, "%d", &contentLength);

			if (strcmp(h[i].n, "Cookie") == 0) {
				if (sscanf(h[i].v, " user=utente%d", &user_index) == 1) {
					new_user = 0; // utente registrato trovato
					printf("%s:%s\n", h[i].n, h[i].v);
				}
			}

			// printf("%s:%s\n", h[i].n, h[i].v);
		}

		if (new_user == 1) { // nuovo utente
			state.last_index++;
			user_index = state.last_index; // assegno un nuovo ID
			state.user_access[user_index] = 1; // inizializzo gli accessi
		} else // utente registrato
			state.user_access[user_index]++; // incremento gli accessi

		printf("DEBUG - Utente %d, numero accessi: %d\n", user_index, state.user_access[user_index]);

		// Analisi della Request Line (es: "GET /index.html HTTP/1.1")
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
			if (new_user == 1) // nuovo utente
				sprintf(response, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nSet-Cookie: user=utente%d; Max-Age=60\r\n\r\n", user_index);
			else
				strcpy(response, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

			// Default sulla home page
			if (strcmp(uri, "/") == 0)
				sprintf(uri, "/index.html");

			// Tentativo di apertura del file richiesto
			int fd = open(uri + 1, O_RDONLY); // uri + 1 per saltare la barra iniziale (es. "/index.html" -> "index.html")

			if (fd < 0)
			{
				// File non trovato: errore 404
				// Se il file non esiste, preparo e invio l'errore 404
				sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html><body>Pagina non trovata!</body></html>");
				sendByte(clientSockId, response, (int)strlen(response));
			}
			else
			{
				// Invio degli header iniziali
				// Chiamo la funzione definita sopra per inviare gli header HTTP al client
				sendByte(clientSockId, response, (int)strlen(response));

				int m = 0;
				char bufferFile[1024];

				// Aggiunta contatore HTML
				char html_accessi[256];
				sprintf(html_accessi, "<html><br> Numero accessi: %d <br>\n", state.user_access[user_index]);

				char chunkHeader[100];
				// Calcola la lunghezza della stringa
				sprintf(chunkHeader, "%x\r\n", (unsigned int)strlen(html_accessi));

				// Invia il chunk artificiale
				sendByte(clientSockId, chunkHeader, (int)strlen(chunkHeader)); // dimensione del chunk
				sendByte(clientSockId, html_accessi, (int)strlen(html_accessi)); // corpo del chunk
				sendByte(clientSockId, "\r\n", 2); // fine chunk

				// Lettura del file e invio a blocchi (chunks)
				// Leggo il file e lo invio a blocchi (chunk) secondo lo standard HTTP/1.1
				while ((m = read(fd, bufferFile, sizeof(bufferFile))) > 0)
				{
					char chunkSize[100];
					// La dimensione del chunk deve essere scritta in esadecimale
					sprintf(chunkSize, "%x\r\n", m); // Scrivo la dimensione del chunk in esadecimale seguita da \r\n

					sendByte(clientSockId, chunkSize, (int)strlen(chunkSize)); // Invio dimensione chunk
					sendByte(clientSockId, bufferFile, m);                     // Invio contenuto chunk
					sendByte(clientSockId, "\r\n", 2);                         // Ogni chunk termina con CRLF
				}

				// Invio del chunk finale di dimensione 0 per terminare la risposta chunked
				// Invio il chunk vuoto di dimensione 0 per indicare al client la fine della trasmissione
				sendByte(clientSockId, "0\r\n\r\n", 5);
			}

			close(fd);
		}
		// --- METODO NON SUPPORTATO ---
		else {
			sprintf(response, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");

			// Chiamo la funzione definita sopra per inviare la risposta di errore al client
			sendByte(clientSockId, response, (int)strlen(response));
		}

		// Chiusura del socket del client (DEVE stare qui, fuori dall'if/else, per chiudere sempre la connessione)
		close(clientSockId);

	} // Chiude il ciclo while

	return 0;
} // Chiude il main
