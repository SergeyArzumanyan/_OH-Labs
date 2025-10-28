#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/stat.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main() {
    int semid = semget(IPC_PRIVATE, 4, IPC_CREAT | 0600);
    if (semid == -1) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    printf("Semaphore set created: semid=%d\n", semid);

    unsigned short values[4] = {2, 2, 2, 2};
    union semun arg;
    arg.array = values;

    if (semctl(semid, 0, SETALL, arg) == -1) {
        perror("semctl SETALL failed");
        exit(EXIT_FAILURE);
    }

    printf("Initial values set: 2 2 2 2\n");
    return 0;
}
