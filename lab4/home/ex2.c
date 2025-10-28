#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {

    int shmid1 = shmget(IPC_PRIVATE, 100*1024, S_IRUSR | S_IWUSR);
    int shmid2 = shmget(IPC_PRIVATE, 200*1024, S_IRUSR | S_IWUSR);

    if (shmid1 == -1 || shmid2 == -1) { perror("shmget"); exit(1); }

    void *addr1 = shmat(shmid1, NULL, 0);
    void *addr2 = shmat(shmid2, NULL, 0);

    if (addr1 == (void*)-1 || addr2 == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    printf("Shared memory segments attached:\n");
    printf("Segment 1 at %p\n", addr1);
    printf("Segment 2 at %p\n", addr2);

    pid_t pid = getpid();
    printf("PID: %d\n", pid);

    printf("Press Ctrl+Z to suspend the process...\n");

    pause(); 

    return 0;
    
}
