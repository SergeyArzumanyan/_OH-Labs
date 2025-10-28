#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <semid>\n", argv[0]);
        return 1;
    }

    int semid = atoi(argv[1]);

    struct sembuf op_decr = {0, -3, 0};  

    printf("Process %d: trying to decrement semaphore by 3...\n", getpid());
    if (semop(semid, &op_decr, 1) == -1) {
        perror("semop failed");
        return 1;
    }
    int val = semctl(semid, 0, GETVAL);
    printf("Process %d: decremented by 3. Current value: %d\n", getpid(), val);



    return 0;
}
