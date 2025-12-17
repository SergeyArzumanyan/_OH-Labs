/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o task5 task5.c
 * 2. Create a test file: echo "Hello" > test.txt
 * 3. Read: ./task5 test.txt
 * 4. Try to update (won't affect file): ./task5 test.txt "NewValue"
 * 
 * This program demonstrates MAP_PRIVATE memory mapping.
 * Changes to private mappings are NOT written back to the file.
 * msync() will typically fail or have no effect with MAP_PRIVATE.
 */

#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API (getpid)
#include <string.h>     // String manipulation functions (strlen, strncpy, memset, strerror)
#include <sys/mman.h>   // Memory mapping functions (mmap, munmap, msync)
#include <fcntl.h>      // File control constants (O_RDWR, O_CREAT)
#include <sys/stat.h>   // File status
#include <errno.h>      // Error numbers (errno, strerror)

#define MEM_SIZE 15     // Size of the memory-mapped region

int main(int argc, char *argv[])
{
    char *addr;         // Pointer to the memory-mapped region
    int fd;             // File descriptor

    // Check if at least one argument (filename) is provided
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file [new-value]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open or create the file with read-write permissions
    fd = open(argv[1], O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Set the file size to MEM_SIZE bytes
    if (ftruncate(fd, MEM_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Map the file into memory with PRIVATE mapping:
    // MAP_PRIVATE = changes are NOT visible to other processes and NOT written to the file
    // This creates a copy-on-write mapping - changes are local to this process
    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Close the file descriptor - the mapping remains valid
    if (close(fd) == -1) {
        perror("close");
        munmap(addr, MEM_SIZE);
        exit(EXIT_FAILURE);
    }

    // Print information about the private mapping
    printf("Mapped %d bytes (MAP_PRIVATE). PID=%d\n", MEM_SIZE, getpid());
    printf("Current content (first bytes): %.*s\n", MEM_SIZE, addr);

    // If a new value is provided, update the mapped memory
    if (argc > 2) {
        // Validate that the new value fits in the buffer
        if (strlen(argv[2]) >= MEM_SIZE) {
            fprintf(stderr, "'new-value' too large\n");
            munmap(addr, MEM_SIZE);
            exit(EXIT_FAILURE);
        }

        // Clear and copy the new value to the mapped memory
        memset(addr, 0, MEM_SIZE);
        strncpy(addr, argv[2], MEM_SIZE - 1);

        // Attempt to synchronize - this will typically fail or have no effect
        // with MAP_PRIVATE because changes are not meant to be written back
        if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
            fprintf(stderr, "msync returned error: %s\n", strerror(errno));
        }

        // Note: The change is only in this process's memory, not in the file
        printf("Copied \"%s\" to private mapping (not written to file)\n", argv[2]);
    }

    // Unmap the memory region
    munmap(addr, MEM_SIZE);
    return 0;
}
