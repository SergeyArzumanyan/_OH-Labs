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

int main() {
    int shmid = shmget(SHM_KEY, BUF_SIZE, 0666);
    int semid = semget(SEM_KEY, 2, 0666);

    char *shmp = shmat(shmid, NULL, SHM_RDONLY);

    ssize_t transfers = 0;
    while (1) {
        semop(semid, &(struct sembuf){READ_SEM, -1, 0}, 1);
        if (shmp[0] == 0) break; 
        write(STDOUT_FILENO, shmp, strnlen(shmp, BUF_SIZE));
        semop(semid, &(struct sembuf){WRITE_SEM, 1, 0}, 1); 
        transfers++;
    }

    shmdt(shmp);
    printf("\nReader done (%zd transfers).\n", transfers);
    return 0;
}
