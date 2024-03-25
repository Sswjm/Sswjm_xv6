#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int readline(char *args[MAXARG], int curr_argc)
{
    char buf[1024];
    int n = 0;
    int offset = 0;

    while (read(0, buf + n, 1)) {
        if (n == 1023) {
            fprintf(2, "argument is too long\n");
            exit(1);
        }
        if (buf[n] == '\n') {
            break;
        }

        n++;
    }

    buf[n] = 0;
    if (n == 0) return 0;
    
    while (offset < n) {
        args[curr_argc++] = buf + offset;
        while (buf[offset] != ' ' && offset < n) offset++;
        while (buf[offset] == ' ' && offset < n) buf[offset++] = 0;
    }
    
    return curr_argc;
}

int main(int argc, char **argv)
{
    int i, curr_argc;
    char *command;
    char *args[MAXARG];

    if (argc == 1) {
        fprintf(2, "xagrs: too few arguments\n");
        exit(1);
    }

    // get xargs command from argv
    command = (char *)malloc(strlen(argv[1]) + 1); 
    strcpy(command, argv[1]);

    // get xargs arguments for the command
    for (i = 1;i < argc;i++) {
        args[i - 1] = (char *)malloc(strlen(argv[i]) + 1);
        strcpy(args[i - 1], argv[i]);
    }

    while ((curr_argc = readline(args, argc - 1)) != 0) {
        args[curr_argc] = 0;
        if (fork() == 0) {
            exec(command, args);
            fprintf(2, "exec failed\n");
            exit(1);
        }
        wait(0);
    }

    exit(0);
}