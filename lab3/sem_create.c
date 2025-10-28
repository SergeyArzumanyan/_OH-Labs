#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "sem.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage error: the program accepts exactly one argument - the number of semaphores in the set.\n");
        exit(EXIT_FAILURE);
    }

    union semun arg;
    int n = atoi(argv[1]);

  
    key_t key = ftok("sem.h", 65);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

   
    int semid = semget(key, n, IPC_CREAT | S_IRUSR | S_IWUSR);

    if (semid == -1) {
        fprintf(stderr, "semid error\n");
        exit(EXIT_FAILURE);
    }

    printf("Semaphore ID = %d\n", semid);
    exit(EXIT_SUCCESS);
}

