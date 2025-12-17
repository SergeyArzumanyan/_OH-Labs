/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o task8 task8.c -lpthread
 * 2. Run: ./task8
 * 3. Type messages and press Enter. Press Ctrl+D to finish.
 * 
 * This program demonstrates inter-process communication using:
 * - Shared memory (mmap with MAP_ANONYMOUS)
 * - Semaphores for synchronization between parent and child processes
 * The child process writes messages, and the parent process reads and displays them.
 */

#include <stdio.h>      // Standard I/O functions (printf, fgets)
#include <stdlib.h>     // Standard library functions (exit, EXIT_FAILURE)
#include <unistd.h>     // POSIX API (fork, write, STDOUT_FILENO)
#include <string.h>     // String manipulation functions (strlen, strnlen)
#include <sys/mman.h>   // Memory mapping functions (mmap, munmap, MAP_ANONYMOUS)
#include <semaphore.h>  // Semaphore functions (sem_init, sem_wait, sem_post)
#include <sys/wait.h>   // Process control (wait)

#define BUF_SIZE 256    // Size of the shared memory buffer

int main() {
    // Create shared memory buffer for message passing
    // MAP_ANONYMOUS = memory is not backed by a file
    // MAP_SHARED = changes are visible to other processes
    char *shmp = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shmp == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }

    // Create shared semaphore for write synchronization
    // This semaphore controls when the writer can write
    sem_t *write_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    // Create shared semaphore for read synchronization
    // This semaphore signals when data is ready to be read
    sem_t *read_sem  = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Initialize write semaphore to 1 (writer can write initially)
    // Second argument (1) means the semaphore is shared between processes
    sem_init(write_sem, 1, 1);
    // Initialize read semaphore to 0 (no data ready initially)
    sem_init(read_sem, 1, 0);  

    // Create a child process
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (pid == 0) {
        // CHILD PROCESS: Writer
        printf("Enter messages (Ctrl+D to finish):\n");
        // Read messages from stdin until EOF (Ctrl+D)
        while (fgets(shmp, BUF_SIZE, stdin) != NULL) {
            // Wait for permission to write (decrement write_sem)
            sem_wait(write_sem);   
            // Copy the message is already done by fgets above
            // Signal that data is ready to read (increment read_sem)
            sem_post(read_sem);    
        }

        // Send termination signal: empty string
        sem_wait(write_sem);
        shmp[0] = '\0';  // Set first character to null terminator
        sem_post(read_sem);

        // Clean up: unmap all shared memory regions
        munmap(shmp, BUF_SIZE);
        munmap(write_sem, sizeof(sem_t));
        munmap(read_sem, sizeof(sem_t));
        exit(0);
    } else {
        // PARENT PROCESS: Reader
        ssize_t transfers = 0;  // Count of messages transferred
        while (1) {
            // Wait for data to be available (decrement read_sem)
            sem_wait(read_sem);
            // Check for termination signal (empty string)
            if (shmp[0] == '\0') break;
            // Write the message to stdout
            write(STDOUT_FILENO, shmp, strnlen(shmp, BUF_SIZE));
            // Add newline if message doesn't end with one
            if (shmp[strlen(shmp)-1] != '\n') write(STDOUT_FILENO, "\n", 1);
            // Signal that writer can write again (increment write_sem)
            sem_post(write_sem);
            transfers++;
        }
        printf("Reader done (%zd transfers).\n", transfers);

        // Wait for child process to finish
        wait(NULL);

        // Clean up: unmap all shared memory regions
        munmap(shmp, BUF_SIZE);
        munmap(write_sem, sizeof(sem_t));
        munmap(read_sem, sizeof(sem_t));
    }

    return 0;
}
