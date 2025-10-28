#include <stdio.h>
#include <sys/sem.h>

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;

    int semid = atoi(argv[1]);
    struct semid_ds buf;

    if (semctl(semid, 0, IPC_STAT, &buf) == -1) {
        perror("semctl IPC_STAT failed");
        return 1;
    }

    printf("Semaphore set ID: %d\n", semid);
    printf("Current number of processes waiting for semaphore to increase: %lu\n",
                (unsigned long) buf.semncnt); 
    return 0;
}
