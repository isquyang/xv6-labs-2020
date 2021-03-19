#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int firstPair[2], secondPair[2];

    int retCode = pipe(firstPair);
    if (retCode != 0) {
        printf("create first pipe failed, errorCode=%d\n", retCode);
        exit(-1);
    }

    retCode = pipe(secondPair);
    if (retCode != 0) {
        printf("create second pipe failed, errorCode=%d\n", retCode);
        exit(-1);
    }

    int readNumber, writeNumber = 1;
    retCode = fork();
    if (retCode < 0) {
        printf("do dork failed, errorCode=%d\n", retCode);
    } else if (retCode == 0) {
        close(firstPair[1]);
        close(secondPair[0]);

        retCode = read(firstPair[0], &readNumber, sizeof(readNumber));
        if (retCode != sizeof(readNumber)) {
            printf("child read failed, errorCode=%d\n", retCode);
            exit(-1);
        }

        printf("%d: received ping\n", getpid());

        retCode = write(secondPair[1], &writeNumber, sizeof(writeNumber));
        if (retCode != sizeof(readNumber)) {
            printf("child write failed, errorCode=%d\n", retCode);
            exit(-1);
        }
    } else {
        close(firstPair[0]);
        close(secondPair[1]);

        retCode = write(firstPair[1], &writeNumber, sizeof(writeNumber));
        if (retCode != sizeof(readNumber)) {
            printf("parent write failed, errorCode=%d\n", retCode);
            exit(-1);
        }

        retCode = read(secondPair[0], &readNumber, sizeof(readNumber));
        if (retCode != sizeof(readNumber)) {
            printf("parent read failed, errorCode=%d\n", retCode);
            exit(-1);
        }
        printf("%d: received pong\n", getpid());
    }
    exit(0);
}
