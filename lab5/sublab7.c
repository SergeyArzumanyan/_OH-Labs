/*
 * HOW TO RUN ON macOS:
 * 1. Compile without MAP_ANON: gcc -o sublab7 sublab7.c
 * 2. Compile with MAP_ANON: gcc -DUSE_MAP_ANON -o sublab7 sublab7.c
 * 3. Run: ./sublab7
 * 
 * This program demonstrates shared anonymous memory mapping between parent and child processes.
 * Two methods are shown:
 * - Using /dev/zero (default)
 * - Using MAP_ANONYMOUS flag (when USE_MAP_ANON is defined)
 * The child process increments a shared integer, and the parent sees the change.
 */

#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API (fork, getpid)
#include <sys/wait.h>   // Process control (wait)
#include <sys/mman.h>   // Memory mapping functions (mmap, munmap, MAP_ANONYMOUS)
#include <fcntl.h>      // File control constants (O_RDWR)

int main(int argc, char *argv[])
{
    int *addr;  // Pointer to shared memory region (will hold an integer)

#ifdef USE_MAP_ANON
    // Method 1: Use MAP_ANONYMOUS flag (Linux/Unix style, works on macOS)
    // This creates anonymous shared memory without needing a file
    // MAP_ANONYMOUS = memory is not backed by any file
    // -1 = file descriptor is ignored when using MAP_ANONYMOUS
    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
#else
    // Method 2: Use /dev/zero (works on macOS and Linux)
    // /dev/zero is a special device that provides zero-filled memory
    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Map /dev/zero into memory - this gives us zero-initialized shared memory
    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Close the file descriptor - the mapping remains valid
    close(fd);
#endif

    // Initialize the shared integer to 1
    *addr = 1; 
    printf("Parent initializes value = %d\n", *addr);

    // Create a child process
    switch (fork()) {
        case -1:
            // Fork failed
            perror("fork");
            exit(EXIT_FAILURE);

        case 0: 
            // Child process
            // The child sees the value initialized by the parent
            printf("Child sees initial value = %d\n", *addr);
            // Increment the shared value
            (*addr)++;
            printf("Child increments value to %d\n", *addr);

            // Unmap the memory in the child process
            if (munmap(addr, sizeof(int)) == -1) {
                perror("munmap in child");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);

        default: 
            // Parent process
            // Wait for the child process to complete
            if (wait(NULL) == -1) {
                perror("wait");
                exit(EXIT_FAILURE);
            }

            // The parent sees the value modified by the child
            // This demonstrates that MAP_SHARED makes changes visible across processes
            printf("Parent sees value after child = %d\n", *addr);

            // Unmap the memory in the parent process
            if (munmap(addr, sizeof(int)) == -1) {
                perror("munmap in parent");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
    }
}
