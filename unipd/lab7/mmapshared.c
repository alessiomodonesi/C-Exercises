#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILENAME "sharedfile"
#define FILESIZE 4096  // sufficiente per il messaggio "Hello world!"

int main() {
    // Crea un file da condividere
    int fd = open(FILENAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    // Allunga il file a FILESIZE
    if (ftruncate(fd, FILESIZE) == -1) {
        perror("ftruncate");
        close(fd);
        exit(1);
    }

    // Mappa il file in memoria condivisa
    char *shared_mem = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }

    // Chiude il file descriptor, mappatura già avvenuta
    close(fd);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        munmap(shared_mem, FILESIZE);
        exit(1);
    } else if (pid == 0) {
        // Processo figlio
        const char *msg = "Hello world!";
        sprintf(shared_mem, "%s", msg);  // Scrive nella memoria condivisa
        exit(0);  // Termina
    } else {
        // Processo padre
        wait(NULL);  // Attende che il figlio termini

        // Legge e stampa il messaggio
        printf("Messaggio dal figlio: %s\n", shared_mem);

        // Pulizia
        munmap(shared_mem, FILESIZE);
        remove(FILENAME);
    }

    return 0;
}
