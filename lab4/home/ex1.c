#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main() {
    
    int shmid1 = shmget(IPC_PRIVATE, 100*1024, S_IRUSR | S_IWUSR);
    if (shmid1 == -1) { perror("shmget 100KB"); exit(1); }
    printf("Shared memory 1 ID: %d\n", shmid1);

    int shmid2 = shmget(IPC_PRIVATE, 200*1024, S_IRUSR | S_IWUSR);
    if (shmid2 == -1) { perror("shmget 200KB"); exit(1); }
    printf("Shared memory 2 ID: %d\n", shmid2);

    return 0;
}
