/**
 * SO Lab 2 - Task 2
 */

  #include <stdio.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/wait.h>

  int main()
  {
    pid_t res;

    /* fork a child process */
    res = fork();
    if (res < 0) { 
      /* error occurred */ 
      fprintf(stderr, "fork failed!\n"); 
      return 1;
    }
    else if (res == 0) { /* child process */
      printf("[child] res = %d\n", res); /* value returned by fork() */
      printf("[child] current pid = %d\n", getpid()); /* current process' pid */
    }
    else { /* parent process */
      usleep(10000); /* wait 10 ms */
      printf("[parent] res = %d\n", res);  /* value returned by fork() */
      printf("[parent] current pid = %d\n", getpid()); /* current process' pid */
      wait(NULL); /* wait for child to complete... */
    }

    return 0;
  }
