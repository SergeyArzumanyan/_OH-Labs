/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o sublab9 sublab9.c
 * 2. Create a test file: echo "Hello, World!" > source.txt
 * 3. Run: ./sublab9 source.txt destination.txt
 * 4. Verify: cat destination.txt
 * 
 * This program demonstrates file copying using read() and write() system calls.
 * It copies data from a source file to a destination file in chunks.
 * This is a basic implementation of the 'cp' command functionality.
 */

#include <stdio.h>      // Standard I/O functions (fprintf, perror)
#include <stdlib.h>     // Standard library functions (EXIT_FAILURE, EXIT_SUCCESS)
#include <unistd.h>     // POSIX API (read, write, close)
#include <fcntl.h>      // File control constants (O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC)
#include <errno.h>      // Error numbers

#define BUF_SIZE 4096   // Buffer size for reading/writing (4KB is a common page size)

int main(int argc, char *argv[]) {
    // Check if exactly two arguments (source and destination) are provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Store file names for clarity
    char *src_file = argv[1];   // Source file path
    char *dest_file = argv[2];  // Destination file path

    // Open the source file in read-only mode
    int src_fd = open(src_file, O_RDONLY);
    if (src_fd < 0) {
        perror("Error opening source file");
        return EXIT_FAILURE;
    }

    // Open or create the destination file in write-only mode
    // O_CREAT = create the file if it doesn't exist
    // O_TRUNC = truncate the file to zero length if it exists
    // 0666 = read-write permissions for owner, group, and others
    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd < 0) {
        perror("Error opening/creating destination file");
        close(src_fd);  // Clean up source file descriptor
        return EXIT_FAILURE;
    }

    // Buffer for reading and writing data
    char buffer[BUF_SIZE];
    ssize_t bytes_read;  // Number of bytes read (ssize_t can be negative for errors)

    // Read from source file in chunks until end of file (read returns 0)
    while ((bytes_read = read(src_fd, buffer, BUF_SIZE)) > 0) {
        ssize_t bytes_written = 0;  // Track how many bytes we've written
        
        // Write loop: ensure all bytes are written (write() may not write everything at once)
        while (bytes_written < bytes_read) {
            // Write remaining bytes to destination
            // buffer + bytes_written = pointer to remaining data
            // bytes_read - bytes_written = number of bytes remaining to write
            ssize_t n = write(dest_fd, buffer + bytes_written, bytes_read - bytes_written);
            if (n < 0) {
                // Write error occurred
                perror("Error writing to destination file");
                close(src_fd);
                close(dest_fd);
                return EXIT_FAILURE;
            }
            // Update the count of bytes written
            bytes_written += n;
        }
    }

    // Check if read() returned an error (negative value)
    if (bytes_read < 0) {
        perror("Error reading source file");
    }

    // Close both file descriptors
    close(src_fd);
    close(dest_fd);

    return EXIT_SUCCESS;
}
