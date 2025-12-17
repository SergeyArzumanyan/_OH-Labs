/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o sublab1 sublab1.c
 * 2. Create a test file: echo "Hello, World!" > test.txt
 * 3. Run: ./sublab1 test.txt
 * 
 * This program demonstrates memory-mapped file reading using mmap().
 * It maps a file into memory and writes its contents to stdout.
 */

#include <stdio.h>      // Standard I/O functions (printf, fprintf, etc.)
#include <stdlib.h>     // Standard library functions (exit, EXIT_FAILURE, etc.)
#include <unistd.h>     // POSIX API (STDOUT_FILENO, write, etc.)
#include <sys/mman.h>   // Memory mapping functions (mmap, MAP_FAILED, etc.)
#include <sys/stat.h>   // File status structure (struct stat)
#include <fcntl.h>      // File control constants (O_RDONLY)

int main(int argc, char *argv[])
{
	char *addr;         // Pointer to the memory-mapped region
	int fd;             // File descriptor for the opened file
	struct stat sb;     // Structure to hold file statistics (size, permissions, etc.)

	// Check if exactly one command-line argument (filename) is provided
	if (argc != 2) {
		fprintf(stderr, "Usage error: %s file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Open the file in read-only mode
	fd = open(argv[1], O_RDONLY);

	// Check if file opening failed
	if (fd == -1) {
		fprintf(stderr, "open error");
		exit(EXIT_FAILURE);
	}

	// Get file statistics, particularly the file size (st_size)
	if (fstat(fd, &sb) == -1) {
		fprintf(stderr, "fstat error");
		exit(EXIT_FAILURE);
	}

	// Map the file into memory:
	// NULL = let kernel choose the address
	// sb.st_size = map the entire file size
	// PROT_READ = memory is readable only
	// MAP_PRIVATE = changes are not visible to other processes
	// fd = file descriptor
	// 0 = offset from start of file
	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	// Check if memory mapping failed
	if (addr == MAP_FAILED) {
		fprintf(stderr, "mmap error");
		exit(EXIT_FAILURE);
	}

	// Write the entire mapped memory region to stdout
	// STDOUT_FILENO = file descriptor for standard output
	if (write(STDOUT_FILENO, addr, sb.st_size) != sb.st_size) {
		fprintf(stderr, "partial/failed write");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
