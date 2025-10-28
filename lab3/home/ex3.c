#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main() {
    int key = 1234; 
    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0600);
        if (semid != -1) {
            printf("Child 1 (PID %d): Successfully created semaphore set.\n", getpid());
            union semun arg;
            struct sembuf sop;
            arg.val = 0;
            if (semctl(semid, 0, SETVAL, arg) == -1) {
                perror("semctl");
                exit(EXIT_FAILURE);
            }
            sop.sem_num = 0; sop.sem_op = 0; sop.sem_flg = 0;
            if (semop(semid, &sop, 1) == -1) { perror("semop"); exit(EXIT_FAILURE); }
            printf("Child 1 (PID %d): Performed no-op semop to set sem_otime.\n", getpid());
        } else {
            printf("Child 1 (PID %d): Failed to create semaphore: %s\n", getpid(), strerror(errno));
        }
        exit(0);
    }

    pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) { 
        sleep(1); 
        int semid = semget(key, 1, 0600);
        if (semid == -1) { perror("semget"); exit(EXIT_FAILURE); }

        struct semid_ds ds;
        union semun arg;
        arg.buf = &ds;

        const int MAX_TRIES = 10;
        for (int i = 0; i < MAX_TRIES; i++) {
            if (semctl(semid, 0, IPC_STAT, arg) == -1) { perror("semctl"); exit(EXIT_FAILURE); }
            if (ds.sem_otime != 0) {
                printf("Child 2 (PID %d): semop() performed by other process. Safe to use semaphore.\n", getpid());
                break;
            }
            sleep(1);
        }
        if (ds.sem_otime == 0) {
            fprintf(stderr, "Child 2 (PID %d): Existing semaphore not initialized!\n", getpid());
            exit(EXIT_FAILURE);
        }
        exit(0);
    }

    wait(NULL);
    wait(NULL);

    return 0;
}
