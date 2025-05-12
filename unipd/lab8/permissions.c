#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main() {
    char filename[100];
    struct stat fileStat;

    /* Read the filename from the user */
    printf("Enter the filename: ");
    scanf("%s", filename);

    /* Get the current file permissions */
    stat(filename, &fileStat);

    /* Print the current file permissions */
    printf("Current file permissions: %o\n", fileStat.st_mode & 0777);

    /* Change the file permissions */
    chmod(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    /* Get the updated file permissions */
    stat(filename, &fileStat);

    /* Print the updated file permissions */
    printf("Updated file permissions: %o\n", fileStat.st_mode & 0777);

    return 0;
}
