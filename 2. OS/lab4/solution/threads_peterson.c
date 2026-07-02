/**
 * SO Lab 4 - Task 1
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

long sum; /* this data is shared by the threads */
bool flag[2];
int turn;

void *runner1(void *param); /* first thread */
void *runner2(void *param); /* second thread */

int main(int argc, char *argv[])
{
  pthread_t tid1, tid2; /* the thread identifiers */
  pthread_attr_t attr;  /* set of attributes for the threads */

  /* default attributes */
  pthread_attr_init(&attr);

  sum = 0;
  flag[0] = true;
  flag[1] = true;
  turn = 0;

  /* create the threads */
  pthread_create(&tid1, &attr, runner1, argv[1]);
  pthread_create(&tid2, &attr, runner2, argv[1]);

  /* now wait for the threads to exit */
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  printf("sum = %d\n", sum);
}

/**
 * The first thread will begin control in this function
 */
void *runner1(void *param)
{
  long i, upper = atol(param);

  if (upper > 0)
  {
    for (i = 1; i <= upper; i++)
    {
      flag[0] = true;
      turn = 1;
      while (flag[1] && turn == 1)
        usleep(1); /* wait */
      /***** critical section *****/
      sum++;
      /***** end of critical section ****/
      flag[0] = false;
    }
  }

  pthread_exit(0);
}

/**
 * The second thread will begin control in this function
 */
void *runner2(void *param)
{
  long i, upper = atol(param);

  if (upper > 0)
  {
    for (i = 1; i <= upper; i++)
    {
      flag[1] = true;
      turn = 0;
      while (flag[0] && turn == 0)
        ; /* wait */
      /***** critical section *****/
      sum--;
      /***** end of critical section ****/
      flag[1] = false;
    }
  }

  pthread_exit(0);
}
