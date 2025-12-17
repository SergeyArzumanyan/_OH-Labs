/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o task3 task3.c
 * 2. Create a test file: echo "Hello" > test.txt
 * 3. Run: ./task3 test.txt "NewValue"
 * 
 * This program is similar to task2 but includes a 5-second sleep delay.
 * This demonstrates that the mapping remains valid even after closing the file descriptor.
 * The sleep allows you to observe the behavior or test with another process.
 */

#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API (sleep function)
#include <string.h>     // String manipulation functions
#include <sys/mman.h>   // Memory mapping functions
#include <fcntl.h>      // File control constants
#define MEM_SIZE 15     // Size of the memory-mapped region

int main(int argc, char *argv[])
{
	char *addr;         // Pointer to the memory-mapped region
	int fd;             // File descriptor

	// Check if at least one argument (filename) is provided
	if (argc < 2) {
        fprintf(stderr, "Usage error: %s file [new-value]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Open the file in read-write mode
	fd = open(argv[1], O_RDWR);

	// Check if file opening failed
	if (fd == -1) {
		fprintf(stderr, "open error");
		exit(EXIT_FAILURE);
	}

	// Map the file into memory with read and write permissions
	// MAP_SHARED makes changes visible to other processes and persistent to the file
	addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	// Check if memory mapping failed
	if (addr == MAP_FAILED) {
		fprintf(stderr, "mmap error");
		exit(EXIT_FAILURE);
	}

	// Sleep for 5 seconds - demonstrates that mapping persists after file descriptor is closed
	// During this time, you could potentially access the file from another process
    sleep(5);

	// Close the file descriptor - the mapping remains valid
	if (close(fd) == -1) { 
		fprintf(stderr, "close error");
		exit(EXIT_FAILURE);
	}

	// Print the current content of the mapped memory region
    printf("Current string=%.*s\n", MEM_SIZE, addr);

	// If a new value is provided, update the mapped memory
	if (argc > 2) { 
		// Validate that the new value fits in the buffer
		if (strlen(argv[2]) >= MEM_SIZE) {
			fprintf(stderr, "'new-value' too large\n");
			exit(EXIT_FAILURE);
		}

		// Clear the mapped region and copy the new value
		memset(addr, 0, MEM_SIZE); 
		strncpy(addr, argv[2], MEM_SIZE - 1);

		// Synchronize changes to disk
		if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
			fprintf(stderr, "msync");
			exit(EXIT_FAILURE);
		}

		printf("Copied \"%s\" to shared memory\n", argv[2]);
	}

	exit(EXIT_SUCCESS);
}
