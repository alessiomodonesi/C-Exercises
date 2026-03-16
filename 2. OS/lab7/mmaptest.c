#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define FILENAME "testfile"
#define FILESIZE (1024*1024*100) /* 100MB */


int main() {
    /* Create a test file */
    int fd = open(FILENAME, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, FILESIZE); /* set the length of the file */

    /* Map the file to memory */
    char *data = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    /* Close the file */
    close(fd);

    /* Initialise the random number generator */
    srand(time(NULL));

    char byte;

    /* Access the file randomly using mmap() */
    clock_t start = clock(); /* start counting */
    for (int i = 0; i < 1000000; i++) {
        off_t offset = rand() % FILESIZE; /* generate random offset */
        byte = data[offset]; /* read byte from memory mapped file */
    }
    clock_t end = clock(); /* stop counting */
    printf("mmap() random access time: %.2f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);



    /* Access the file randomly using standard system calls */
    fd = open(FILENAME, O_RDONLY); /* open the file */
    start = clock(); /* start counting */
    for (int i = 0; i < 1000000; i++) {
        off_t offset = rand() % FILESIZE; /* generate random offset */
        lseek(fd, offset, SEEK_SET); /* point to desired byte */
        read(fd, &byte, 1); /* read byte from file */
    }
    end = clock(); /* stop counting */
    printf("Standard system calls random access time: %.2f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);



    /* Clean up */
    close(fd);
    munmap(data, FILESIZE);
    remove(FILENAME);

    return 0;
}
