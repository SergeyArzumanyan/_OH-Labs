#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define BUF_SIZE 256
#define WRITE_SEM 0
#define READ_SEM 1

union semun { int val; };

int main() {
    int shmid = shmget(SHM_KEY, BUF_SIZE, IPC_CREAT | 0666);
    int semid = semget(SEM_KEY, 2, IPC_CREAT | 0666);

    char *shmp = shmat(shmid, NULL, 0);

    union semun arg;
    arg.val = 1;
    semctl(semid, WRITE_SEM, SETVAL, arg); 
    semctl(semid, READ_SEM, SETVAL, arg);  

    printf("Write messages (Ctrl+D to finish):\n");

    ssize_t n;
    while ((n = read(STDIN_FILENO, shmp, BUF_SIZE)) > 0) {
        semop(semid, &(struct sembuf){WRITE_SEM, -1, 0}, 1); 
        shmp[n] = '\0'; 
        semop(semid, &(struct sembuf){READ_SEM, 1, 0}, 1);   
    }

    shmp[0] = 0; 
    semop(semid, &(struct sembuf){READ_SEM, 1, 0}, 1); 

    shmdt(shmp);
    printf("Writer done.\n");
    return 0;
}
