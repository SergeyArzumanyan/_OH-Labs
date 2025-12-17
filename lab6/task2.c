/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o task2 task2.c
 * 2. Create a test file with initial content: echo "Hello" > test.txt
 * 3. Read the file: ./task2 test.txt
 * 4. Update the file: ./task2 test.txt "NewValue"
 * 
 * This program demonstrates shared memory mapping with read/write access.
 * It can read and modify file contents through memory mapping.
 */

#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API
#include <string.h>     // String manipulation functions (strlen, strncpy, memset)
#include <sys/mman.h>   // Memory mapping functions (mmap, msync)
#include <fcntl.h>      // File control constants (O_RDWR)
#define MEM_SIZE 15     // Size of the memory-mapped region (15 bytes)

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

	// Map the file into memory with read and write permissions:
	// PROT_READ | PROT_WRITE = memory is both readable and writable
	// MAP_SHARED = changes are visible to other processes and written to the file
	addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	// Check if memory mapping failed
	if (addr == MAP_FAILED) {
		fprintf(stderr, "mmap error");
		exit(EXIT_FAILURE);
	}

	// Close the file descriptor - the mapping remains valid after closing
	if (close(fd) == -1) { 
		fprintf(stderr, "close error");
		exit(EXIT_FAILURE);
	}

	// Print the current content of the mapped memory region
	// %.*s prints up to MEM_SIZE characters
    printf("Current string=%.*s\n", MEM_SIZE, addr);

	// If a new value is provided as second argument, update the mapped memory
	if (argc > 2) { 
		// Check if the new value is too large for our buffer
		if (strlen(argv[2]) >= MEM_SIZE) {
			fprintf(stderr, "'new-value' too large\n");
			exit(EXIT_FAILURE);
		}

		// Clear the entire mapped region (set all bytes to 0)
		memset(addr, 0, MEM_SIZE); 
		// Copy the new string to the mapped memory (leaving space for null terminator)
		strncpy(addr, argv[2], MEM_SIZE - 1);

		// Synchronize the mapped memory with the file on disk
		// MS_SYNC = synchronous write, waits until data is written to disk
		if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
			fprintf(stderr, "msync");
			exit(EXIT_FAILURE);
		}

		printf("Copied \"%s\" to shared memory\n", argv[2]);
	}

	exit(EXIT_SUCCESS);
}
