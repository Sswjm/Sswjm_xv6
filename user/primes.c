#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void newProcess(int *p)
{
    int prime;
    int flag;
    int n;

    close(p[1]); // close the write end of the pipe
    if (read(p[0], &prime, 4) != 4) {
        fprintf(2, "failed to read prime from the pipe");
        exit(1);
    }

    printf("prime %d\n", prime);

    flag = read(p[0], &n, 4);
    if (flag) {
        int p1[2];
        pipe(p1);
        if (fork() == 0) {
            newProcess(p1);
        }
        else {
            close(p1[0]);
            if (n % prime) {
                write(p1[1], &n, 4);
            }

            while (read(p[0], &n, 4)) {
                if (n % prime) {
                    write(p1[1], &n, 4);
                }
            }

            close(p[0]);
            close(p1[1]);
            wait(0);
        }
    }

    exit(0);
}

int main()
{
    int p[2];
    pipe(p); // create a pipe 
    if (fork() == 0) {
        // child process;
        newProcess(p);
    }
    else {
        close(p[0]);  // close the read end of the pipe
        for (int i = 2;i <= 35;i++) {
            if (write(p[1], &i, 4) != 4) {
                fprintf(2, "number %d failed to pass the pipe\n", i);
                exit(1);
            }
        }

        close(p[1]);
        wait(0);
    }

    exit(0);
}