//
// Created by yang.qu on 2021/3/18.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define DIRSIZE 256

char*
fName(char *path)
{
    static char buf[DIRSIZE+1];
    char *p;

    memset(buf, 0, sizeof(buf));

    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    if(strlen(p) >= DIRSIZE){
        return p;
    }
    memmove(buf, p, strlen(p));
    //memset(buf+strlen(p), '\0', DIRSIZE-strlen(p));
    return buf;
}

void
find(char *path, char *findFileName)
{
    // fprintf(1, "path=%s,findFileName=%s\n", path, findFileName);

    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }


    switch(st.type) {
        case T_FILE: {
            char *fileName = fName(path);

            //fprintf(1, "path=%s,fileName=%s\n", path, fileName);

            if (strcmp(fileName, findFileName) == 0) {
                fprintf(1, "%s\n", path);
            }
            close(fd);
            break;
        }
        case T_DIR: {
            if (strlen(path) + 1 + DIRSIZE + 1 > sizeof(buf)) {
                printf("find: path too long\n");
                break;
            }
            while(read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) {
                    continue;
                }
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                    continue;
                }

                memset(buf, 0, sizeof(buf));
                strcpy(buf, path);
                p = buf+strlen(buf);
                *p++ = '/';
                memmove(p, de.name, DIRSIZE);
                find(buf, findFileName);
            }
            close(fd);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(2, "usage: find $PATH $NAME\n");
        exit(0);
    }

    find(argv[1], argv[2]);
    exit(0);
}
