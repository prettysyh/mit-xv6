#include "kernel/types.h"
#include "user/user.h"

void runprocess(int fd) {
    int prime = 0;
    int forked = 0;
    int num = 0;
    int pipes[2];
    while (1) {
        int read_bytes = read(fd, &num, 4);

        if (read_bytes == 0) {
            close(fd);
            if (forked) {
                close(pipes[1]);
                int chile_status;
                wait(&chile_status);
            }
            exit(0);
        }

        if (prime == 0) {
            prime = num;
            printf("prime %d\n", prime);
        }

        if (num % prime != 0) {
            if (!forked) {
                pipe(pipes);
                forked = 1;
                int ret = fork();
                if (ret == 0) {
                    // child
                    close(pipes[1]);
                    close(fd);
                    runprocess(pipes[0]);
                } else {
                    close(pipes[0]);
                }
            }
            write(pipes[1], &num, 4);
        }
    }
}

int main(int argc, char *argv[]) {
    int pipes[2];
    pipe(pipes);
    for (int i = 2; i <= 35; i ++ )
        write(pipes[1], &i, 4);
    close(pipes[1]);
    runprocess(pipes[0]);
    exit(0);
}