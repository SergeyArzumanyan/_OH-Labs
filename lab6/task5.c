#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define MEM_SIZE 15

int main(int argc, char *argv[])
{
    char *addr;
    int fd;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s file [new-value]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, MEM_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }

    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("close");
        munmap(addr, MEM_SIZE);
        exit(EXIT_FAILURE);
    }

    printf("Mapped %d bytes (MAP_PRIVATE). PID=%d\n", MEM_SIZE, getpid());
    printf("Current content (first bytes): %.*s\n", MEM_SIZE, addr);

    if (argc > 2) {
        if (strlen(argv[2]) >= MEM_SIZE) {
            fprintf(stderr, "'new-value' too large\n");
            munmap(addr, MEM_SIZE);
            exit(EXIT_FAILURE);
        }

        memset(addr, 0, MEM_SIZE);
        strncpy(addr, argv[2], MEM_SIZE - 1);


        if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
            fprintf(stderr, "msync returned error: %s\n", strerror(errno));
        }

        printf("Copied \"%s\" to private mapping (not written to file)\n", argv[2]);
    }

    munmap(addr, MEM_SIZE);
    return 0;
}
