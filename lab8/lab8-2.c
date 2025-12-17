/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o lab8-2 lab8-2.c
 * 2. Make sure the server (lab8-1) is running first
 * 3. Run the client: ./lab8-2
 * 4. Type messages and press Enter. They will be sent to the server and broadcast to other clients.
 * 5. Messages from other clients will appear automatically.
 * 6. Press Ctrl+D to disconnect
 * 
 * This is a TCP chat client using IPv4.
 * It uses select() to handle both stdin input and socket data simultaneously.
 * Connects to localhost (127.0.0.1) on port 5555.
 */

#include <stdio.h>      // Standard I/O functions (printf, perror, fgets)
#include <stdlib.h>     // Standard library functions (exit)
#include <string.h>     // String manipulation (strlen)
#include <unistd.h>      // POSIX API (read, write, close, STDIN_FILENO)
#include <arpa/inet.h>  // Internet address manipulation (htons, inet_pton)
#include <sys/select.h>  // Select function for I/O multiplexing
#include <sys/socket.h> // Socket functions (socket, connect)

#define PORT 5555       // Server port number
#define BUF_SIZE 1024   // Buffer size for reading messages

int main() {
    int sfd;                    // Socket file descriptor
    struct sockaddr_in addr;     // IPv4 socket address structure
    char buf[BUF_SIZE];         // Buffer for reading/writing messages
    fd_set read_fds;            // Set of file descriptors ready for reading

    // Create a TCP socket (IPv4, connection-oriented)
    // AF_INET = IPv4 address family
    // SOCK_STREAM = reliable, connection-oriented byte stream (TCP)
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd<0){ perror("socket"); exit(1); }

    // Set up the server address structure
    addr.sin_family = AF_INET;              // IPv4 address family
    addr.sin_port = htons(PORT);             // Convert port to network byte order
    // Convert IP address string "127.0.0.1" to network byte order
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);

    // Connect to the server
    if(connect(sfd,(struct sockaddr*)&addr,sizeof(addr))<0){ 
        perror("connect"); 
        exit(1); 
    }

    printf("Connected. Type messages:\n");

    // Main client loop
    while(1){
        // Initialize the file descriptor set for select()
        FD_ZERO(&read_fds);                  // Clear all bits in the set
        FD_SET(STDIN_FILENO,&read_fds);      // Monitor stdin for user input
        FD_SET(sfd,&read_fds);               // Monitor socket for server messages
        int maxfd = sfd;                     // Track the highest file descriptor number

        // Wait for activity on stdin or socket
        // select() blocks until at least one file descriptor is ready
        if(select(maxfd+1,&read_fds,NULL,NULL,NULL)<0){ 
            perror("select"); 
            exit(1); 
        }

        // Check if stdin has input (user typed something)
        if(FD_ISSET(STDIN_FILENO,&read_fds)){
            // Read a line from stdin
            if(fgets(buf,BUF_SIZE,stdin)!=NULL){
                // Send the message to the server
                write(sfd,buf,strlen(buf));
            } else break;  // EOF (Ctrl+D) - exit loop
        }

        // Check if socket has data (message from server/other clients)
        if(FD_ISSET(sfd,&read_fds)){
            // Read message from the server
            ssize_t n = read(sfd,buf,BUF_SIZE-1);
            if(n<=0){ 
                printf("Server closed.\n"); 
                break;  // Server closed connection
            }
            // Null-terminate the received message
            buf[n]='\0';
            // Display the message
            printf("%s",buf);
        }
    }
    // Close the socket connection
    close(sfd);
}
