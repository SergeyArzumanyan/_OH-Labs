#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

#define BUF_SIZE 256

int main() {
    char *shmp = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shmp == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }

    sem_t *write_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *read_sem  = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_init(write_sem, 1, 1);
    sem_init(read_sem, 1, 0);  

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (pid == 0) {
        printf("Enter messages (Ctrl+D to finish):\n");
        while (fgets(shmp, BUF_SIZE, stdin) != NULL) {
            sem_wait(write_sem);   
            sem_post(read_sem);    
        }

        sem_wait(write_sem);
        shmp[0] = '\0';
        sem_post(read_sem);

        munmap(shmp, BUF_SIZE);
        munmap(write_sem, sizeof(sem_t));
        munmap(read_sem, sizeof(sem_t));
        exit(0);
    } else {
        ssize_t transfers = 0;
        while (1) {
            sem_wait(read_sem);
            if (shmp[0] == '\0') break;
            write(STDOUT_FILENO, shmp, strnlen(shmp, BUF_SIZE));
            if (shmp[strlen(shmp)-1] != '\n') write(STDOUT_FILENO, "\n", 1);
            sem_post(write_sem);
            transfers++;
        }
        printf("Reader done (%zd transfers).\n", transfers);

        wait(NULL);

        munmap(shmp, BUF_SIZE);
        munmap(write_sem, sizeof(sem_t));
        munmap(read_sem, sizeof(sem_t));
    }

    return 0;
}
