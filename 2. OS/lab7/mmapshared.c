#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define FILENAME "testfile"
#define FILE_SIZE   4096

int main(void) {
    pid_t pid;
    int fd;
    char *data;
    char *msg = "Hello world!";
    
    /* Open the shared file for read and write access */
    fd = open(FILENAME, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    /* Set the size of the shared file to FILE_SIZE */
    ftruncate(fd, FILE_SIZE);
    
    /* Map the shared file into memory */
    data = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    /* Fork a child process */
    pid = fork();
    if (pid == 0) {
        /* Child process */
        
        /* Write a message to the shared memory */
        sprintf(data, "%s", msg);
        printf("Child process wrote to shared memory: %s\n", data);
        
        /* Exit the child process */
        exit(EXIT_SUCCESS);
    }
    else {
        /* Parent process */
        
        /* Wait for the child process to finish */
        wait(NULL);
        
        /* Read the message from the shared memory */
        printf("Parent process read from shared memory: %s\n", data);
        
        /* Unmap the shared memory and close the file */
        munmap(data, FILE_SIZE);
        close(fd);
    }
    
    return 0;
}
