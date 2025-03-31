/**
 * SO Lab 3 - Task 1
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sum; /* this data is shared by the threads */

void *runner1(void *param); /* first thread */
void *runner2(void *param); /* second thread */

int main(int argc, char *argv[])
{
  pthread_t tid1, tid2; /* the thread identifiers */
  pthread_attr_t attr; /* set of attributes for the threads */

  /* default attributes */
  pthread_attr_init(&attr);

  sum = 0;

  /* create the threads */
  pthread_create(&tid1, &attr, runner1, argv[1]);
  pthread_create(&tid2, &attr, runner2, argv[1]);

  /* now wait for the threads to exit */
  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);

  printf("sum = %d\n",sum);
}

/**
 * The first thread will begin control in this function
 */
void *runner1(void *param) 
{
  int i, upper = atol(param);

  if (upper > 0) {
    for (i = 1; i <= upper; i++)
      sum++; /* increase the shared variable */
  }

    pthread_exit(0);
}

/**
 * The second thread will begin control in this function
 */
void *runner2(void *param) 
{
  int i, upper = atol(param);

  if (upper > 0) {
    for (i = 1; i <= upper; i++)
      sum--; /* decrease the shared variable */
  }

  pthread_exit(0);
}
