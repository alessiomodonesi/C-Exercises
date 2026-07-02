/**
 * SO Lab 3 - Task 3
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 100

int array[ARRAY_SIZE]; /* this data is shared by the threads */

void *runner(void *param); /* the threads are identical */

int main(int argc, char *argv[])
{
    pthread_t tid1, tid2; /* the thread identifiers */
    pthread_attr_t attr;  /* set of attributes for the threads */

    /* default attributes */
    pthread_attr_init(&attr);

    int range1[] = {0, ARRAY_SIZE / 2};          /* data range for 1st thread */
    int range2[] = {ARRAY_SIZE / 2, ARRAY_SIZE}; /* data range for 2nd thread */

    /* create the threads */
    pthread_create(&tid1, &attr, runner, range1);
    pthread_create(&tid2, &attr, runner, range2);

    /* now wait for the threads to exit */
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    for (int i = 0; i < ARRAY_SIZE; i++)
        printf("array[%d] = %d\n", i, array[i]);

    return 0;
}

/**
 * The thread will begin control in this function
 */
void *runner(void *param)
{
    int *range = (int *)param;

    for (int i = range[0]; i <= range[1]; i++)
        array[i] = i * i;

    pthread_exit(0);
}