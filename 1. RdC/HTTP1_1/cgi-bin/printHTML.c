#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *env[])
{
    // 1. STAMPA DI DEBUG DELLE VARIABILI D'AMBIENTE
    // Il server web genitore ha usato setenv() prima di chiamare execv().
    // Qui stampiamo tutto ciò che ci ha passato (es. QUERY_STRING, METHOD, ecc.)
    for (int i = 0; env[i]; i++)
    {
        printf("env: %s\n", env[i]);
    }

    // 2. RECUPERO DEL METODO HTTP
    char *method = getenv("METHOD");

    // Controllo di sicurezza: verifichiamo che il metodo esista prima di fare strcmp
    if (method != NULL && strcmp(method, "GET") == 0)
    {
        printf("Metodo letto: %s\n", method);

        // La write su fd 1 (stdout) finirà direttamente sul socket del client,
        // perché il processo server ha fatto dup2(clientSockId, 1) prima di lanciare questo script!
        write(1, "ciao", 4);

        // Esempio di come potresti generare vera pagina HTML dinamicamente:
        // printf("<html><body>Codice generato dinamicamente via GET!</body></html>");
    }
    else if (method != NULL && strcmp(method, "POST") == 0)
    {
        // 3. GESTIONE DELLA RICHIESTA POST
        // Recuperiamo quanto è lungo il body per sapere quanti byte leggere
        char *contentLength = getenv("ContentLength");
        int contentLengthValue = 0;

        if (contentLength != NULL)
        {
            sscanf(contentLength, "%d", &contentLengthValue);
        }

        // Sicurezza: ci assicuriamo che il body ci sia e che non sia più grande del nostro buffer
        if (contentLengthValue > 0 && contentLengthValue < 100)
        {
            // Inizializzando a 0 {0}, ci assicuriamo che la stringa abbia sempre il terminatore \0
            char buffer[100] = {0};

            // Leggiamo da fd 0 (stdin). Anche questo è stato dirottato dal server (dup2(clientSockId, 0)).
            // In pratica stiamo leggendo il body della richiesta HTTP direttamente dal client di rete.
            read(0, buffer, contentLengthValue);

            printf("Letto body della POST: %s\n", buffer);
        }
        else
        {
            printf("Errore: ContentLength mancante o body troppo grande per il buffer.\n");
        }
    }

    return 0; // Termina il processo figlio CGI
}