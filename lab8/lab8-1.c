/*
 * HOW TO RUN ON macOS:
 * 1. Compile: gcc -o lab8-1 lab8-1.c
 * 2. Run the server: ./lab8-1
 * 3. In other terminals, connect clients using: nc localhost 5555
 *    Or use the lab8-2 client program
 * 4. Type messages in client terminals - they will be broadcast to all other clients
 * 5. Press Ctrl+C to stop the server
 * 
 * This is a TCP chat server using IPv4.
 * It uses select() for multiplexing multiple client connections.
 * Messages from one client are broadcast to all other connected clients.
 */

#include <stdio.h>      // Standard I/O functions (printf, perror)
#include <stdlib.h>     // Standard library functions (exit)
#include <string.h>     // String manipulation (memset)
#include <unistd.h>      // POSIX API (read, write, close)
#include <arpa/inet.h>  // Internet address manipulation (htons, INADDR_ANY)
#include <sys/select.h>  // Select function for I/O multiplexing
#include <sys/socket.h> // Socket functions (socket, bind, listen, accept)

#define PORT 5555           // Server port number
#define MAX_CLIENTS 10     // Maximum number of concurrent clients
#define BUF_SIZE 1024      // Buffer size for reading messages

int main() {
    int sfd;                        // Server socket file descriptor
    int clients[MAX_CLIENTS];       // Array to store client file descriptors
    struct sockaddr_in addr;         // IPv4 socket address structure
    fd_set all_fds;                 // Set of all file descriptors to monitor
    fd_set read_fds;                // Set of file descriptors ready for reading
    char buf[BUF_SIZE];             // Buffer for reading messages
    int i, maxfd;                   // Loop counter and maximum file descriptor

    // Initialize all client slots to -1 (empty)
    for(i=0;i<MAX_CLIENTS;i++) clients[i]=-1;

    // Create a TCP socket (IPv4, connection-oriented)
    // AF_INET = IPv4 address family
    // SOCK_STREAM = reliable, connection-oriented byte stream (TCP)
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd<0){ perror("socket"); exit(1); }

    // Set up the server address structure
    addr.sin_family = AF_INET;              // IPv4 address family
    addr.sin_port = htons(PORT);             // Convert port to network byte order
    addr.sin_addr.s_addr = INADDR_ANY;       // Listen on all network interfaces
    memset(addr.sin_zero,0,sizeof(addr.sin_zero));  // Zero out padding bytes

    // Bind the socket to the address
    if(bind(sfd,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("bind"); exit(1); }
    
    // Mark the socket as a listening socket
    // 5 = maximum number of pending connections in the queue
    if(listen(sfd,5)<0){ perror("listen"); exit(1); }

    // Initialize the file descriptor set for select()
    FD_ZERO(&all_fds);           // Clear all bits in the set
    FD_SET(sfd,&all_fds);         // Add server socket to the set
    maxfd=sfd;                     // Track the highest file descriptor number

    printf("Chat server on port %d...\n", PORT);

    // Main server loop
    while(1){
        // Copy all_fds to read_fds (select() modifies the set)
        read_fds = all_fds;
        
        // Wait for activity on any file descriptor in the set
        // select() blocks until at least one file descriptor is ready
        if(select(maxfd+1,&read_fds,NULL,NULL,NULL)<0){ perror("select"); exit(1); }

        // Check if the server socket has a new connection
        if(FD_ISSET(sfd,&read_fds)){
            // Accept a new client connection
            int cfd = accept(sfd,NULL,NULL);
            if(cfd>=0){
                // Find an empty slot in the clients array
                for(i=0;i<MAX_CLIENTS;i++){ 
                    if(clients[i]<0){ 
                        clients[i]=cfd; 
                        break; 
                    } 
                }
                // Update maxfd if necessary
                if(cfd>maxfd) maxfd=cfd;
                // Add the new client socket to the set
                FD_SET(cfd,&all_fds);
                printf("Client connected.\n");
            }
        }

        // Check all client sockets for incoming data
        for(i=0;i<MAX_CLIENTS;i++){
            int fd = clients[i];
            if(fd<0) continue;  // Skip empty slots
            
            // Check if this client has data to read
            if(FD_ISSET(fd,&read_fds)){
                // Read message from the client
                ssize_t n = read(fd,buf,BUF_SIZE-1);
                if(n<=0){
                    // Client disconnected or error occurred
                    close(fd);
                    FD_CLR(fd,&all_fds);  // Remove from the set
                    clients[i]=-1;        // Mark slot as empty
                    printf("Client disconnected.\n");
                } else {
                    // Null-terminate the received message
                    buf[n]='\0';
                    // Broadcast the message to all other clients
                    for(int j=0;j<MAX_CLIENTS;j++){
                        if(clients[j]>=0 && clients[j]!=fd) 
                            write(clients[j],buf,n);
                    }
                    printf("Msg: %s",buf);
                }
            }
        }
    }
}
