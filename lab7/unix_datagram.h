/*
 * unix_datagram.h
 * Header file for Unix domain datagram socket programs
 * 
 * Defines:
 * - SV_SOCK_PATH: Path where the server socket will be created
 * - BUF_SIZE: Size of the buffer for reading/writing data
 */

#ifndef UNIX_DATAGRAM_H
#define UNIX_DATAGRAM_H

#define SV_SOCK_PATH "/tmp/ud_ucase_sv"  // Server socket file path
#define BUF_SIZE 1024                    // Buffer size for data transfer

#endif

