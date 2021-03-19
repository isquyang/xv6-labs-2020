#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
isPrime(int number)
{
    int i = 2;
    for ( ; i < number; i++ ) {
        if (number % i == 0) {
            return 0;
        }
    }
    return 1;
}

void
childFunc(int *leftPair)
{
    close(leftPair[1]);

    int firstNumberFlag = 1;
    int hasForkFlag = 0;
    int number;
    int rightPair[2];
    int waitNumber;

    for (; ;) {
        int retCode = read(leftPair[0], &number, sizeof(number));
        if (retCode == 0) {
            break;
        }

        if (firstNumberFlag) {
            printf("prime %d\n", number);
            firstNumberFlag = 0;
        } else {
            if (!hasForkFlag) {
                pipe(rightPair);

                retCode = fork();
                if (retCode == 0) {
                    childFunc(rightPair);
                } else {
                    close(rightPair[0]);

                    write(rightPair[1], &number, sizeof(number));
                    hasForkFlag = 1;
                }
            } else {
                write(rightPair[1], &number, sizeof(number));
            }
        }
    }
    close(rightPair[1]);
    wait(&waitNumber);
    exit(0);
}

int
main(int argc, char *argv[])
{
    printf("prime 2\n");

    int leftPair[2];
    pipe(leftPair);

    int waitNumber;
    int pid = fork();
    if (pid == 0) {
        childFunc(leftPair);
    } else {
        close(leftPair[0]);

        int i = 3;
        for (; i <= 35; i++) {
            if (isPrime(i)) {
                write(leftPair[1], &i, sizeof(i));
            }
        }
        close(leftPair[1]);
        wait(&waitNumber);
    }
    exit(0);
}
