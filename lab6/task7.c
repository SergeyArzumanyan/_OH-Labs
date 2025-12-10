#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int *addr; 

#ifdef USE_MAP_ANON

addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
#else

int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
#endif

    *addr = 1; 
    printf("Parent initializes value = %d\n", *addr);

    switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0: 
            printf("Child sees initial value = %d\n", *addr);
            (*addr)++;
            printf("Child increments value to %d\n", *addr);

            if (munmap(addr, sizeof(int)) == -1) {
                perror("munmap in child");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);

        default: 
            if (wait(NULL) == -1) {
                perror("wait");
                exit(EXIT_FAILURE);
            }

            printf("Parent sees value after child = %d\n", *addr);

            if (munmap(addr, sizeof(int)) == -1) {
                perror("munmap in parent");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
    }
}
