/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o lab7-3client lab7-3client.c
 * 2. Make sure the server (lab7-3) is running first
 * 3. Run the client with messages as arguments: ./lab7-3client "hello" "world" "test"
 * 4. The server will convert messages to uppercase and send them back
 * 
 * This is a Unix domain datagram socket client.
 * It sends messages to the server and receives uppercase responses.
 * Uses connectionless (UDP-like) communication via Unix domain sockets.
 */

#include <stdio.h>      // Standard I/O functions (printf, fprintf)
#include <stdlib.h>     // Standard library functions (exit, EXIT_FAILURE)
#include <unistd.h>     // POSIX API (getpid)
#include <string.h>     // String manipulation (strlen, strncpy, strcmp, snprintf)
#include "unix_datagram.h" // Header file with socket path and buffer size definitions

int main(int argc, char *argv[])
{
    struct sockaddr_un svaddr;  // Server socket address structure
    struct sockaddr_un claddr;  // Client socket address structure
    int sfd;                    // Socket file descriptor
    int j;                      // Loop counter
    size_t msgLen;              // Length of message to send
    ssize_t numBytes;           // Number of bytes received
    char resp[BUF_SIZE];        // Buffer for receiving responses

    // Check if at least one message argument is provided
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage error\n");
        exit(EXIT_FAILURE);
    }

    // Create a Unix domain datagram socket
    // AF_UNIX = Unix domain (local communication)
    // SOCK_DGRAM = connectionless, unreliable datagram service
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (sfd == -1) {
        fprintf(stderr, "socket error\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the client socket address structure
    // For datagram sockets, the client also needs to bind to an address
    // We use a unique pathname based on the process ID
    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    // Create a unique socket path for this client using its process ID
    snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_ucase_cl.%ld", (long) getpid());

    // Bind the client socket to its unique address
    // This allows the server to know where to send responses
    if (bind(sfd, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un)) == -1) {
        fprintf(stderr, "bind error\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the server socket address structure
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    // Copy the server socket path (must match the server's path)
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    // Send each command-line argument as a message to the server
    for (j = 1; j < argc; j++) {
        // Get the length of the message (may be longer than BUF_SIZE)
        msgLen = strlen(argv[j]);
        
        // Send the message to the server
        // sendto() sends data to the address specified in svaddr
        if (sendto(sfd, argv[j], msgLen, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) != msgLen) {
            fprintf(stderr, "sendto error\n");
            exit(EXIT_FAILURE);
        }

        // Receive the response from the server
        // NULL, NULL means we don't need the server's address
        numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);

        if (numBytes == -1) {
            fprintf(stderr, "recvfrom error\n");
            exit(EXIT_FAILURE);
        }

        // Print the server's response (uppercase version of the message)
        printf("Response %d: %.*s\n", j, (int) numBytes, resp);
    }

    // Clean up: remove the client socket file
    remove(claddr.sun_path);
    exit(EXIT_SUCCESS);
}

