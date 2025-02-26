#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

void signal_handler(int signal) 
{
  printf("\nSegnale catturato");
  exit(0);
}

int main(int argc, char *argv[])
{
    int pid = fork();
    if (pid == 0)
    {
        printf("\nIn attesa di un segnale...\n");
        while(1)
        {
            signal(SIGINT, signal_handler);
        }
    }
    wait(&pid);
    return 0;
}