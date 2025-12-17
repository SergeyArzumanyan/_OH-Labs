/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o lab7-1client lab7-1client.c
 * 2. Make sure the server (lab7-1) is running first
 * 3. Run the client: ./lab7-1client
 * 4. Type messages and press Enter. They will be sent to the server.
 * 5. Press Ctrl+D to finish sending and disconnect
 * 
 * This is a Unix domain stream socket client.
 * It connects to the server and sends messages from stdin.
 * Uses connection-oriented (TCP-like) communication via Unix domain sockets.
 */

#include <stdio.h>      // Standard I/O functions (fprintf, fgets)
#include <stdlib.h>     // Standard library functions (exit, EXIT_FAILURE)
#include <unistd.h>     // POSIX API (read, write, close)
#include <errno.h>      // Error numbers
#include <string.h>     // String manipulation (memset, strncpy, strlen)
#include "unix_stream.h" // Header file with socket path and buffer size definitions

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;  // Unix domain socket address structure
    int sfd;                  // Socket file descriptor
    ssize_t numRead;           // Number of bytes read
    char buf[BUF_SIZE];        // Buffer for reading data

    // Create a Unix domain stream socket
    // AF_UNIX = Unix domain (local communication)
    // SOCK_STREAM = reliable, connection-oriented byte stream
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        fprintf(stderr, "socket error\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the socket address structure for the server
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;  // Set address family to Unix domain
    // Copy the server socket path (must match the server's path)
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    // Connect to the server
    if (connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        fprintf(stderr, "connect error\n");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Type messages (Ctrl+D to finish):\n");

    // Read messages from stdin and send them to the server
    while (fgets(buf, BUF_SIZE, stdin) != NULL) {
        // Send the message to the server
        if (write(sfd, buf, strlen(buf)) == -1) {
            fprintf(stderr, "write error\n");
            exit(EXIT_FAILURE);
        }
    }

    // Close the socket connection
    close(sfd);
    return 0;
}

