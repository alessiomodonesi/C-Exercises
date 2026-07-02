#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *writer();
void *reader();

unsigned char book = 'a';

int main()
{
    pthread_t tidw, tidr1, tidr2;

    pthread_attr_t attr;
    pthread_attr_init(&attr); /* default thread attributes */

    pthread_create(&tidw, &attr, writer, NULL);  /* create writer thread */
    pthread_create(&tidr1, &attr, reader, NULL); /* create reader thread */
    pthread_create(&tidr2, &attr, reader, NULL); /* create reader thread */

    pthread_join(tidw, NULL);
    pthread_join(tidr1, NULL);
    pthread_join(tidr2, NULL);

    return 0;
}

void *writer()
{
    while (1)
    {
        /* start critical section */
        book++; /* write*/
        /* end critical section */
        printf("written: %c\n", book);
        usleep(1000);
    }
    pthread_exit(0);
}

void *reader()
{
    unsigned char page;
    while (1)
    {
        /* start critical section */
        page = book; /* read */
        /* end critical section */
        printf("read: %c\n", page);
        usleep(1000);
    }
    pthread_exit(0);
}