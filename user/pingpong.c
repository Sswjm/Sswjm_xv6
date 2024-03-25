#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv)
{
    int p[2];
    int pid;

    pipe(p);
    
    if (fork() == 0) {
        // child process
        pid = getpid();
        char buf[2];
        if (read(p[0], buf, 1) != 1) {
            fprintf(2, "child process failed to read\n");
            exit(1);
        }
        close(p[0]);

        printf("%d: received ping\n", pid);

        if (write(p[1], buf, 1) != 1) {
            fprintf(2, "child process failed to write\n");
            exit(1);
        }
        close(p[1]);
        exit(0);
    }
    else {
        // parent process
        pid = getpid();
        char buf[2];

        if (write(p[1], buf, 1) != 1) {
            fprintf(2, "parent process failed to write\n");
            exit(1);
        }
        close(p[1]);  // close write end

        if(read(p[0], buf, 1) != 1) {
            fprintf(2, "parent process failed to read\n");
            exit(1);
        }
        close(p[0]);  // close read end

        printf("%d: received pong\n", pid);
    }

    exit(0);
}