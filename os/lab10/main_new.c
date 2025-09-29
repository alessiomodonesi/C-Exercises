#include <stdio.h>

/* Include the shell header */
#include "shell.h"

/* Implement the shell function callback here */
static int _board_handler(int argc, char **argv)
{
    /* Not used, avoid a warning during build */
    (void)argc;
    (void)argv;

    puts(RIOT_BOARD);
    return 0;
}

static int _cpu_handler(int argc, char **argv)
{
    /* Not used, avoid a warning during build */
    (void)argc;
    (void)argv;

    puts(RIOT_CPU);
    return 0;
}


static int _echo_handler(int argc, char **argv)
{
    int token = 1;
    while (argc-- > 1)
    {
        printf("%s ", argv[token++]);
    }
    puts("");

    return 0;
}


/* Add the shell command to the list of commands here */
static const shell_command_t shell_commands[] = {
    { "board", "Print the board name", _board_handler },
    { "cpu", "Print the cpu name", _cpu_handler },
    { "echo", "Display a line of text", _echo_handler },
    { NULL, NULL, NULL }
};


int main(void)
{
    /* Start the shell here */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
