/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o lab7-3 lab7-3.c
 * 2. In one terminal, run the server: ./lab7-3
 * 3. In another terminal, run the client: ./lab7-3client "message1" "message2" ...
 * 4. The server will convert messages to uppercase and send them back
 * 5. Press Ctrl+C to stop the server
 * 
 * This is a Unix domain datagram socket server.
 * It receives messages, converts them to uppercase, and sends them back.
 * Uses connectionless (UDP-like) communication via Unix domain sockets.
 */

#include <stdio.h>      // Standard I/O functions (printf, fprintf)
#include <stdlib.h>     // Standard library functions (exit, EXIT_FAILURE)
#include <string.h>     // String manipulation (strlen, strncpy, memset)
#include <unistd.h>     // POSIX API
#include <ctype.h>      // Character classification (toupper)
#include <errno.h>      // Error numbers (ENOENT)
#include <sys/socket.h> // Socket functions (socket, bind, recvfrom, sendto)
#include <sys/un.h>     // Unix domain socket structures
#include "unix_datagram.h" // Header file with socket path and buffer size definitions

int main(int argc, char *argv[])
{
    struct sockaddr_un svaddr;  // Server socket address structure
    struct sockaddr_un claddr;  // Client socket address structure (for receiving)
    int sfd;                    // Server socket file descriptor
    int j;                      // Loop counter
    ssize_t numBytes;           // Number of bytes received/sent
    socklen_t len;              // Length of address structure
    char buf[BUF_SIZE];         // Buffer for receiving and sending data

    // Create a Unix domain datagram socket (connectionless, like UDP)
    // AF_UNIX = Unix domain (local communication)
    // SOCK_DGRAM = connectionless, unreliable datagram service
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        fprintf(stderr, "socket error\n");
        exit(EXIT_FAILURE);
    }

    // Validate that the socket path is not too long
    // Unix domain socket paths have a maximum length (usually 108 bytes)
    if (strlen(SV_SOCK_PATH) > sizeof(svaddr.sun_path) - 1) {
        fprintf(stderr, "Server socket path too long\n");
        exit(EXIT_FAILURE);
    }

    // Remove the socket file if it exists from a previous run
    // ENOENT means "file doesn't exist" - that's okay, we ignore that error
    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        fprintf(stderr, "remove error\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the server socket address structure
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;  // Set address family to Unix domain
    // Copy the socket path (defined in unix_datagram.h)
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    // Bind the socket to the address (associate socket with a file path)
    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1) {
        fprintf(stderr, "bind error\n");
        exit(EXIT_FAILURE);
    }

    // Main server loop: receive messages, process them, and send responses
    for (;;) {
        // Initialize length variable before recvfrom
        len = sizeof(struct sockaddr_un);
        
        // Receive a datagram from a client
        // recvfrom() returns the number of bytes received and fills in claddr with client's address
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &claddr, &len);

        if (numBytes == -1) {
            fprintf(stderr, "recvfrom error\n");
            exit(EXIT_FAILURE);
        }

        // Print information about the received message
        printf("Server received %ld bytes from %s\n", (long) numBytes, claddr.sun_path);

        // Convert all characters in the buffer to uppercase
        for (j = 0; j < numBytes; j++) {
            buf[j] = toupper((unsigned char) buf[j]);
        }

        // Send the converted message back to the client
        // sendto() sends the data to the address specified in claddr
        if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *) &claddr, len) != numBytes) {
            fprintf(stderr, "sendto error\n");
            exit(EXIT_FAILURE);
        }
    }
}

