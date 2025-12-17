/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o sublab4 sublab4.c
 * 2. Run: ./sublab4 test.txt
 * 
 * This program demonstrates SIGBUS signal handling when accessing memory-mapped files.
 * It creates a file, maps it, then shrinks the file to trigger potential SIGBUS errors.
 * Note: On macOS, the behavior may differ from Linux - writes might succeed even after truncation.
 */

#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API (getpid, sleep)
#include <string.h>     // String manipulation functions (memset)
#include <sys/mman.h>   // Memory mapping functions (mmap, munmap, msync)
#include <fcntl.h>      // File control constants (O_RDWR, O_CREAT)
#include <signal.h>     // Signal handling (sigaction, SIGBUS)
#include <sys/stat.h>   // File status
#include <errno.h>      // Error numbers

#define MEM_SIZE 15     // Size of the memory-mapped region

// Signal handler for SIGBUS (bus error signal)
// This signal is typically raised when accessing invalid memory-mapped regions
static void sigbus_handler(int sig, siginfo_t *si, void *unused) {
    (void)unused;       // Suppress unused parameter warning
    fprintf(stderr, "Received signal %d (SIGBUS). si_addr=%p\n", sig, si ? si->si_addr : NULL);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char *addr;         // Pointer to the memory-mapped region
    int fd;             // File descriptor
    struct sigaction sa; // Structure for signal action configuration

    // Check if at least one argument (filename) is provided
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file [new-value]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open or create the file with read-write permissions
    // O_CREAT creates the file if it doesn't exist
    // 0666 = read-write permissions for owner, group, and others
    fd = open(argv[1], O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Set the file size to MEM_SIZE bytes (truncate or extend)
    if (ftruncate(fd, MEM_SIZE) == -1) {
        perror("ftruncate (initial)");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Map the file into memory with read and write permissions
    // MAP_SHARED makes changes visible to other processes
    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Set up signal handler for SIGBUS
    memset(&sa, 0, sizeof(sa));           // Initialize structure to zero
    sa.sa_sigaction = sigbus_handler;     // Set the handler function
    sa.sa_flags = SA_SIGINFO;             // Use extended signal info
    if (sigaction(SIGBUS, &sa, NULL) == -1) {
        perror("sigaction");
        munmap(addr, MEM_SIZE);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Print information about the mapping
    printf("Mapped %d bytes. PID=%d\n", MEM_SIZE, getpid());
    printf("Current content (first bytes): %.*s\n", MEM_SIZE, addr);

    // Shrink the file to 0 bytes - this may cause SIGBUS when accessing mapped memory
    printf("Shrinking underlying file to 0 bytes (ftruncate)...\n");
    if (ftruncate(fd, 0) == -1) {
        perror("ftruncate (shrink)");
        munmap(addr, MEM_SIZE);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Wait a moment for the system to process the truncation
    sleep(1);

    // Attempt to write to the mapped region - this may trigger SIGBUS
    printf("Attempting to write to mapped region to trigger SIGBUS...\n");

    // Use volatile to prevent compiler optimization of the write
    volatile char *p = addr;
    p[0] = 'X';  // Write to the first byte of the mapped region

    // On macOS, this might succeed even after truncation (behavior differs from Linux)
    printf("Write succeeded (no SIGBUS). Current first byte: %c\n", addr[0]);

    // Attempt to synchronize changes to disk (may fail after truncation)
    if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
        perror("msync");
    }
    // Unmap the memory region
    munmap(addr, MEM_SIZE);
    // Close the file descriptor
    close(fd);
    return 0;
}
