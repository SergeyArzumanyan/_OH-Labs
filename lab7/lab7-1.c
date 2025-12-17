/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o lab7-1 lab7-1.c
 * 2. In one terminal, run the server: ./lab7-1
 * 3. In another terminal, run the client: ./lab7-1client
 * 4. Type messages in the client terminal and see them appear in the server terminal
 * 5. Press Ctrl+C to stop the server
 * 
 * This is a Unix domain stream socket server.
 * It receives messages from clients and displays them on stdout.
 * Uses connection-oriented (TCP-like) communication via Unix domain sockets.
 */

#include <stdio.h>      // Standard I/O functions (printf, perror)
#include <stdlib.h>     // Standard library functions (exit, EXIT_FAILURE)
#include <unistd.h>     // POSIX API (read, write, STDOUT_FILENO)
#include <errno.h>      // Error numbers (ENOENT)
#include <string.h>     // String manipulation (memset, strncpy)
#include "unix_stream.h" // Header file with socket path and buffer size definitions

#define BACKLOG 5       // Maximum number of pending connections in the queue

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;  // Unix domain socket address structure
    int sfd;                  // Server socket file descriptor
    int cfd;                  // Client socket file descriptor
    ssize_t numRead;           // Number of bytes read from client
    char buf[BUF_SIZE];        // Buffer for reading data from client

    // Create a Unix domain stream socket (connection-oriented, like TCP)
    // AF_UNIX = Unix domain (local communication)
    // SOCK_STREAM = reliable, connection-oriented byte stream
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Remove the socket file if it exists from a previous run
    // ENOENT means "file doesn't exist" - that's okay, we ignore that error
    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        perror("remove");
        exit(EXIT_FAILURE);
    }

    // Initialize the socket address structure
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;  // Set address family to Unix domain
    // Copy the socket path (defined in unix_stream.h)
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    // Bind the socket to the address (associate socket with a file path)
    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Mark the socket as a listening socket
    // BACKLOG specifies the maximum number of pending connections
    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("SERVER STARTED. Waiting for clients...\n");

    // Main server loop: accept connections and handle them
    for (;;) {
        // Accept a new client connection
        // NULL, NULL means we don't need client address information
        cfd = accept(sfd, NULL, NULL);

        if (cfd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Client connected.\n");

        // Read data from the client until connection is closed (read returns 0)
        while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
            // Write received data to stdout
            write(STDOUT_FILENO, buf, numRead);
        }

        // Check if read() returned an error (not just EOF)
        if (numRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        // Close the client connection
        close(cfd);
        printf("\nClient disconnected.\n");
    }
}





