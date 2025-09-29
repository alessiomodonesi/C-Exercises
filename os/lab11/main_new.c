#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "thread.h"
#include "ztimer.h"

/* Include the mutex header here */
#include "mutex.h"

/* Declare here the struct used to handle the buffer and the mutex */
typedef struct {
    char buffer[128];
    mutex_t lock;
} data_t;
static data_t data;


/* Allocate the writer stack here */
static char writer_stack[THREAD_STACKSIZE_MAIN];

static void *writer_thread(void *arg)
{
    (void) arg;

    while (1) {
        /* acquire the lock here */
        mutex_lock(&data.lock);

        /* slowly print some content in the buffer here */
        size_t p = sprintf(data.buffer, "%s", "one");
        ztimer_sleep(ZTIMER_MSEC, 50);
        sprintf(&data.buffer[p], "%s", " two");

        /* release the mutex here */
        mutex_unlock(&data.lock);


        ztimer_sleep(ZTIMER_MSEC, 100);
    }

    return NULL;
}


/* Allocate the reader stack here */
static char reader_stack[THREAD_STACKSIZE_MAIN];

static void *reader_thread(void *arg)
{
    (void) arg;

    while (1) {
        /* safely read the content of the buffer here */
        mutex_lock(&data.lock);
        printf("%s\n", data.buffer);
        mutex_unlock(&data.lock);

        ztimer_sleep(ZTIMER_MSEC, 250);
    }

    return NULL;
}


static int _print_handler(int argc, char **argv)
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "on") == 0)
            mutex_unlock(&data.lock);
        else if (strcmp(argv[1], "off") == 0)
            mutex_lock(&data.lock);
    } 

    return 0;
}


/* Add the shell command to the list of commands here */
static const shell_command_t shell_commands[] = {
    { "print", "Enable/disable thread printing", _print_handler },
    { NULL, NULL, NULL }
};


int main(void)
{
    puts("RIOT application with thread safe concurrency");

    /* Create the write thread here */
    thread_create(writer_stack, sizeof(writer_stack), THREAD_PRIORITY_MAIN - 1, 0, writer_thread, NULL, "writer thread");
    /* Create the reader thread here */
    thread_create(reader_stack, sizeof(reader_stack), THREAD_PRIORITY_MAIN - 1, 0, reader_thread, NULL, "reader thread");


    /* Start the shell here */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);


    return 0;
}
