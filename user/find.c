#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char* basename(char* pathname) {
    char* prev = 0;
    char* curr = strchr(pathname, '/');
    while (curr != 0) {
        prev = curr;
        curr = strchr(curr + 1, '/');
    }
    return prev;
}

void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_FILE:
        char *f_name = basename(path);
        int match = 1;
        if (f_name == 0 || strcmp(f_name + 1, target) != 0)
            match = 0;
        if (match) {
            printf("%s\n", path);
        }
        close(fd);
        break;

    case T_DIR:
        memset(buf, 0, sizeof buf);
        uint path_len = strlen(path);
        memcpy(buf, path, path_len);
        buf[path_len] = '/';
        p = buf + path_len + 1;
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, target);
        }
        close(fd);
        break;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "usage: find [directory] [target filename]\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}