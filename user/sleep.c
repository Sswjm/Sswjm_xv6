#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv)
{
    int time;

    if (argc != 2) {
        fprintf(2, "sleep: argument error\n");
        exit(1);
    }

    time = atoi(argv[1]);
    sleep(time);

    exit(0);
}