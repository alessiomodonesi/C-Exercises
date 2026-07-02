#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t lock;

void *runner(); /* thread function */

int main()
{
    pthread_t tid1, tid2; /* thread identifiers */

    pthread_attr_t attr;
    pthread_attr_init(&attr); /* default thread attributes */

    pthread_mutex_init(&lock, NULL); /* initialise mutex */

    pthread_create(&tid1, &attr, runner, NULL); /* create 1st thread */
    pthread_create(&tid2, &attr, runner, NULL); /* create 2nd thread */

    pthread_join(tid1, NULL); /* wait end of 1st thread */
    pthread_join(tid2, NULL); /* wait end of 2nd thread */

    return 0;
}

void *runner()
{
    pthread_mutex_lock(&lock); /* lock mutex*/

    printf("START critical section of Thread %lu\n",
           (unsigned long)pthread_self());
    for (int i = 0; i < 1000; i++)
    {
        /* do something */
        usleep(10);
    }
    printf("END critical section of Thread %lu\n",
           (unsigned long)pthread_self());

    pthread_mutex_unlock(&lock); /* release mutex */
    pthread_exit(0);
}