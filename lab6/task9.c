#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *src_file = argv[1];
    char *dest_file = argv[2];

    int src_fd = open(src_file, O_RDONLY);
    if (src_fd < 0) {
        perror("Error opening source file");
        return EXIT_FAILURE;
    }

    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd < 0) {
        perror("Error opening/creating destination file");
        close(src_fd);
        return EXIT_FAILURE;
    }

    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(src_fd, buffer, BUF_SIZE)) > 0) {
        ssize_t bytes_written = 0;
        while (bytes_written < bytes_read) {
            ssize_t n = write(dest_fd, buffer + bytes_written, bytes_read - bytes_written);
            if (n < 0) {
                perror("Error writing to destination file");
                close(src_fd);
                close(dest_fd);
                return EXIT_FAILURE;
            }
            bytes_written += n;
        }
    }

    if (bytes_read < 0) {
        perror("Error reading source file");
    }

    close(src_fd);
    close(dest_fd);

    return EXIT_SUCCESS;
}
