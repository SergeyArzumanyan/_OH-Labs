/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o task6 task6.c
 * 2. Create a test file: echo "Hello" > test.txt
 * 3. Run: ./task6 test.txt "NewValue"
 * 
 * WARNING: This program demonstrates accessing memory after unmapping (undefined behavior).
 * The line "addr[0]" after munmap() will likely cause a segmentation fault or access invalid memory.
 * This is intentional to demonstrate the danger of using unmapped memory pointers.
 */

#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API (getpid)
#include <string.h>     // String manipulation functions (memset, strncpy)
#include <sys/mman.h>   // Memory mapping functions (mmap, munmap)
#include <fcntl.h>      // File control constants (O_RDWR, O_CREAT)

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

    // Map the file into memory with read and write permissions
    // MAP_SHARED makes changes visible to other processes and persistent to the file
    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Close the file descriptor - the mapping remains valid
    close(fd);

    // Print information about the mapping
    printf("Mapped %d bytes. PID=%d\n", MEM_SIZE, getpid());
    printf("Current content (first bytes): %.*s\n", MEM_SIZE, addr);

    // If a new value is provided, update the mapped memory
    if (argc > 2) {
        // Clear and copy the new value
        memset(addr, 0, MEM_SIZE);
        strncpy(addr, argv[2], MEM_SIZE - 1);
        printf("Copied \"%s\" to mapped memory\n", argv[2]);
    }

    // Unmap the memory region - after this, addr is no longer valid
    if (munmap(addr, MEM_SIZE) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    printf("Mapping unmapped.\n");

    // WARNING: This is undefined behavior!
    // Accessing memory after unmapping will likely cause a segmentation fault
    // This demonstrates why you should never use a pointer after munmap()
    printf("Trying to read after munmap: %c\n", addr[0]);

    return 0;
}
