#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/stat.h>

union semun {
    int val;             
    struct semid_ds *buf;  
    unsigned short *array; 
    struct seminfo *__buf; 
};

int main()
{
    union semun arg;
    int semid;

    semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
    if (semid == -1) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    arg.val = 2;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL failed");
        exit(EXIT_FAILURE);
    }

    printf("Semaphore ID = %d, initial value = %d\n", semid, arg.val);

    return 0;
}
