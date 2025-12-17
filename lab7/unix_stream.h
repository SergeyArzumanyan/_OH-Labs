/*
 * unix_stream.h
 * Header file for Unix domain stream socket programs
 * 
 * Defines:
 * - SV_SOCK_PATH: Path where the server socket will be created
 * - BUF_SIZE: Size of the buffer for reading/writing data
 */

#ifndef UNIX_STREAM_H
#define UNIX_STREAM_H

#include <sys/socket.h>  // Socket functions and structures
#include <sys/un.h>      // Unix domain socket address structure
#include <string.h>      // String manipulation functions

#define SV_SOCK_PATH "/tmp/unix_stream_socket"  // Server socket file path
#define BUF_SIZE 100                             // Buffer size for data transfer

#endif

