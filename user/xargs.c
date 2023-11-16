#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define buf_size 512

int main(int argc, char *argv[]) {
    char buf[buf_size + 1] = {0};
    char* xargv[MAXARG] = {0};
    int stdin_end = 0;
    uint occupy = 0;

    for (int i = 1; i < argc; i ++ ) {
        xargv[i - 1] = argv[i];
    }

    while (!(stdin_end && occupy == 0)) {
        if (!stdin_end) {
            int remain_size = buf_size - occupy;
            int read_bytes = read(0, buf + occupy, remain_size);
            if (read_bytes < 0) {
                fprintf(2, "xargs: read returns -1 error\n");
            }
            if (read_bytes == 0) {
                close(0);
                stdin_end = 1;
            }
            occupy += read_bytes;
        }

        char *line_end = strchr(buf, '\n');
        while (line_end) {
            char xbuf[buf_size + 1] = {0};
            memcpy(xbuf, buf, line_end - buf);
            xargv[argc - 1] = xbuf;
            int ret = fork();
            if (ret == 0) {
                /* child */
                if (!stdin_end) {
                    close(0);
                }
                if (exec(argv[1], xargv) < 0) {
                    fprintf(2, "xargs: exec fails with -1\n");
                    exit(1);
                }
            } else {
                /* parent */
                memmove(buf, line_end + 1, occupy - (line_end - buf) + 1);
                occupy -= line_end - buf + 1;
                memset(buf + occupy, 0, buf_size - occupy);

                int pid;
                wait(&pid);
                line_end = strchr(buf, '\n');
            }
        }
    }
    exit(0);
}