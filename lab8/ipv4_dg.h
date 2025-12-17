/*
 * ipv4_dg.h
 * Header file for IPv4 datagram socket programs
 * 
 * Defines:
 * - BUF_SIZE: Size of the buffer for reading/writing data
 * - PORT_NUM: Server port number
 * - errExit(): Error handling function
 * 
 * Includes necessary headers for IPv4 socket programming
 */

#include <stdio.h>      // Standard I/O functions (fprintf, stderr)
#include <stdlib.h>     // Standard library functions (exit, EXIT_FAILURE)
#include <netinet/in.h> // Internet address structures (struct sockaddr_in)
#include <arpa/inet.h>  // Internet address manipulation functions (htons, inet_pton)
#include <sys/socket.h> // Socket functions and structures
#include <ctype.h>      // Character classification functions (toupper, etc.)

#define BUF_SIZE 10         // Buffer size for data transfer
#define PORT_NUM 50002       // Server port number

// Error handling function
// Prints an error message and exits the program
void errExit(char *err)
{
    fprintf(stderr, "%s error\n", err);
    exit(EXIT_FAILURE);
}
