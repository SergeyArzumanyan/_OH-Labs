#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 5555
#define BUF_SIZE 1024

int main() {
    int sfd;
    struct sockaddr_in addr;
    char buf[BUF_SIZE];
    fd_set read_fds;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd<0){ perror("socket"); exit(1); }

    addr.sin_family=AF_INET;
    addr.sin_port=htons(PORT);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
    memset(addr.sin_zero,0,sizeof(addr.sin_zero));

    if(connect(sfd,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("connect"); exit(1); }

    printf("Connected to server. Type messages (Ctrl+D to quit):\n");

    while(1){
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO,&read_fds);
        FD_SET(sfd,&read_fds);
        int maxfd=sfd;

        if(select(maxfd+1,&read_fds,NULL,NULL,NULL)<0){ perror("select"); break; }

        if(FD_ISSET(STDIN_FILENO,&read_fds)){
            if(fgets(buf,BUF_SIZE,stdin)==NULL) break;
            write(sfd,buf,strlen(buf));
        }

        if(FD_ISSET(sfd,&read_fds)){
            ssize_t n = read(sfd,buf,BUF_SIZE-1);
            if(n<=0){ printf("Server closed.\n"); break; }
            buf[n]='\0'; printf("%s",buf);
        }
    }

    close(sfd);
    return 0;
}
