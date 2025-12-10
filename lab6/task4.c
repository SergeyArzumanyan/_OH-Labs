#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>

#define MEM_SIZE 15

static void sigbus_handler(int sig, siginfo_t *si, void *unused) {
    (void)unused;
    fprintf(stderr, "Received signal %d (SIGBUS). si_addr=%p\n", sig, si ? si->si_addr : NULL);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char *addr;
    int fd;
    struct sigaction sa;

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
        perror("ftruncate (initial)");
        close(fd);
        exit(EXIT_FAILURE);
    }

    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sigbus_handler;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGBUS, &sa, NULL) == -1) {
        perror("sigaction");
        munmap(addr, MEM_SIZE);
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Mapped %d bytes. PID=%d\n", MEM_SIZE, getpid());
    printf("Current content (first bytes): %.*s\n", MEM_SIZE, addr);

    printf("Shrinking underlying file to 0 bytes (ftruncate)...\n");
    if (ftruncate(fd, 0) == -1) {
        perror("ftruncate (shrink)");
        munmap(addr, MEM_SIZE);
        close(fd);
        exit(EXIT_FAILURE);
    }

    sleep(1);

    printf("Attempting to write to mapped region to trigger SIGBUS...\n");


    volatile char *p = addr;
    p[0] = 'X';  

    printf("Write succeeded (no SIGBUS). Current first byte: %c\n", addr[0]);

    if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
        perror("msync");
    }
    munmap(addr, MEM_SIZE);
    close(fd);
    return 0;
}
