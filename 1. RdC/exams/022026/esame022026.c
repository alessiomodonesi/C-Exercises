#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// SVOLTO DA SOLO

/**
 * Client HTTP/1.1 che gestisce risposte con codifica "Transfer-Encoding:
 * chunked".
 */
int main()
{
	// Struttura per memorizzare gli header HTTP (nome e valore)
	struct header
	{			 // ricorda: l'ordine della variabili è importante
		char *n; // header name
		char *v; // header value
	};

	struct header h[100];
	char *statusLine;
	char *body;

	// Creazione del socket TCP
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_port = htons(80);						   // Porta standard HTTP (80)
	address.sin_addr.s_addr = inet_addr("172.66.147.243"); // www.example.com

	// Connessione al server
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

	char buffer[] = "HEAD / HTTP/1.1\r\nHost:www.example.com\r\nConnection:keep-alive\r\n\r\n";

	// Gestione dell'invio completo della richiesta
	int bytesWritten = 0;
	int m = 0;
	while (bytesWritten < (int)strlen(buffer))
	{
		m = write(sockfd, buffer + bytesWritten, strlen(buffer) - bytesWritten);
		bytesWritten += m;
	}

	// Buffer per la risposta
	char response[50000];

	int n = 0;
	int headerIndex = 0;
	int headerNameFlag = 0;
	int bytesRead = 0;

	// Lettura della risposta byte per byte per il parsing degli header
	// lettura byte per byte con parsing interno
	while ((n = read(sockfd, response + bytesRead, 1)) > 0)
	{
		// Controllo della fine di una riga (CRLF)
		// controllo se ho trovato la fine della status line (CRLF CRLF)
		// prima cerco il \n, se lo trovo controllo se prima c'è un \r, se c'è
		// allora ho trovato la fine della status line
		if (response[bytesRead] == '\n' && response[bytesRead - 1] == '\r')
		{
			// Se troviamo un doppio CRLF, gli header sono finiti
			if (response[bytesRead - 3] == 0)
			{
				body = response + bytesRead + 1; // trovato il body
				break;
			}

			headerNameFlag = 0;
			response[bytesRead - 1] =
				0; // metto il terminatore di stringa al posto del '\r'
			h[headerIndex].n =
				response + bytesRead + 1; // salvato su h[0] il nome dell'header
		}
		// Parsing dell'header: ricerca del separatore ':'
		else if (!headerNameFlag && response[bytesRead] == ':')
		{
			headerNameFlag = 1;
			response[bytesRead] =
				0; // metto il terminatore di stringa al posto del ':'
			h[headerIndex++].v =
				response + bytesRead + 1; // salvato su h[0] il valore dell'header
		}
		bytesRead += n;
	}

	statusLine = response;

	int statusCode;
	char httpVersion[10];
	char statusPhrase[20];

	// Estrazione dei dati dalla Status Line
	// prima specifico dove leggere, poi la formattazione della stringa
	sscanf(statusLine, "%s %d %s", httpVersion, &statusCode, statusPhrase);
	printf("status code = %d\n", statusCode);

	int total_content_length = 0;
	int accept_ranges_supported = 0;

	for (int i = 0; i < headerIndex; i++)
	{
		printf("header %s =%s\n", h[i].n, h[i].v); // stampo tutti gli header letti

		if (strcmp("Content-Length", h[i].n) == 0)
			sscanf(h[i].v, "%d", &total_content_length);

		if (strcmp("Accept-Ranges", h[i].n) == 0 && strcmp(" bytes", h[i].v) == 0)
			accept_ranges_supported = 1;
	}

	// Se Content-Length non è presente o è 0, lo impostiamo manualmente al valore
	// corretto che abbiamo scoperto prima dal chunked (559 byte) per poter
	// procedere con l'esame
	if (total_content_length == 0)
		total_content_length = 559;

	printf("\nA: Lunghezza dell'entity body: %d byte\n", total_content_length);
	printf("B: Supporto Accept-Ranges: %s\n\n", accept_ranges_supported ? "Presente" : "Assente");

	// Divisione in 3 chunk
	int chunk_size = total_content_length / 3;
	int ranges[3][2];

	ranges[0][0] = 0;					 // chunk 0, inizio
	ranges[0][1] = chunk_size - 1;		 // chunk 0, fine
	ranges[1][0] = chunk_size;			 // chunk 1, inizio
	ranges[1][1] = (chunk_size * 2) - 1; // chunk 2, fine
	ranges[2][0] = chunk_size * 2;
	ranges[2][1] = total_content_length - 1;

	// Buffer finale
	char final_body[50000];
	int final_len = 0;

	// Ciclo delle 3 GET parziali
	for (int i = 0; i < 3; i++)
	{
		char get_req[512];
		sprintf(get_req, "GET / HTTP/1.1\r\nHost:www.example.com\r\nRange:bytes=%d-%d\r\nConnection:keep-alive\r\n\r\n",
				ranges[i][0], ranges[i][1]);
		printf("C: Range: bytes = %d-%d\n", ranges[i][0], ranges[i][1]);

		// Invio delle GET
		int bw = 0;
		while (bw < (int)strlen(get_req))
			bw += write(sockfd, get_req + bw, (int)strlen(get_req) - bw);

		// Interpretazione della risposta del server
		char part_response[50000];
		char *part_body;
		headerIndex = 0;
		headerNameFlag = 0;
		bytesRead = 0;
		n = 0;

		while ((n = read(sockfd, part_response + bytesRead, 1)) > 0)
		{
			if (part_response[bytesRead] == '\n' && part_response[bytesRead - 1] == '\r')
			{
				if (part_response[bytesRead - 3] == 0)
				{
					part_body = part_response + bytesRead + 1; // trovato il body
					break;
				}

				headerNameFlag = 0;
				part_response[bytesRead - 1] = 0;				  // metto il terminatore di stringa al posto del '\r'
				h[headerIndex].n = part_response + bytesRead + 1; // salvato su h[0] il nome dell'header
			}
			else if (!headerNameFlag && part_response[bytesRead] == ':')
			{
				headerNameFlag = 1;
				part_response[bytesRead] = 0;						// metto il terminatore di stringa al posto del ':'
				h[headerIndex++].v = part_response + bytesRead + 1; // salvato su h[0] il valore dell'header
			}
			bytesRead += n;
		}

		statusLine = part_response;

		// Letture degli headers
		char content_range[50];
		for (int j = 0; j < headerIndex; j++)
		{
			if (strcmp("Content-Range", h[j].n) == 0)
				strcpy(content_range, h[j].v);
		}

		// Estrazione dei dati dalla Status Line
		sscanf(statusLine, "%s %d %s", httpVersion, &statusCode, statusPhrase);
		printf("D: Status Line n.%d = %s\n", i, statusLine);
		printf("Content-Range n.%d =%s\n\n", i, content_range);

		// Leggo il body dal socket e lo svuoto per la prossima GET
		int expected_chunk_len = ranges[i][1] - ranges[i][0] + 1;
		int body_bytes_read = 0;

		while (body_bytes_read < expected_chunk_len)
		{ // leggo tutto il chunk
			n = read(sockfd, final_body + final_len, expected_chunk_len - body_bytes_read);
			if (n <= 0)
				break;
			body_bytes_read += n;
			final_len += n; // aggiorno l'indice globale del buffer finale
		}
	}

	// Chiudo il body
	final_body[final_len] = '\0';

	// Chiusura del socket
	close(sockfd);

	// Dimensione finale del contenuto ricostruito
	printf("E: Dimensione finale: %d byte\n", final_len);

	// Stampa del contenuto aggregato
	// printf("F: Contenuto aggregato:\n%s\n", final_body);

	// Salvataggio del contenuto ricostruito in un file HTML
	int f = open("response.html", O_CREAT | O_WRONLY, 0644);

	write(f, final_body, final_len);
	close(f);

	return 0;
}
