#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <semid>\n", argv[0]);
        return 1;
    }

    int semid = atoi(argv[1]);

    struct sembuf op = {0, -3, 0};

    if (semop(semid, &op, 1) == -1) {
        perror("semop failed");
        return 1;
    }

    int val = semctl(semid, 0, GETVAL);
    printf("Semaphore decremented by 3. Current value: %d\n", val);

    return 0;
}
