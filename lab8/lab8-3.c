#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 5555
#define BUF_SIZE 1024
#define MAX_CLIENTS 10

int main() {
    int sfd, clients[MAX_CLIENTS], opt=1;
    struct sockaddr_in addr;
    char buf[BUF_SIZE];
    fd_set all_fds, read_fds;
    int i, maxfd;

    for(i=0;i<MAX_CLIENTS;i++) clients[i]=-1;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd<0){ perror("socket"); exit(1); }

    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family=AF_INET;
    addr.sin_port=htons(PORT);
    addr.sin_addr.s_addr=INADDR_ANY;
    memset(addr.sin_zero,0,sizeof(addr.sin_zero));

    if(bind(sfd,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("bind"); exit(1); }
    if(listen(sfd,5)<0){ perror("listen"); exit(1); }

    FD_ZERO(&all_fds);
    FD_SET(sfd,&all_fds);
    maxfd=sfd;

    printf("TCP Echo server running on port %d...\n", PORT);

    while(1){
        read_fds = all_fds;
        if(select(maxfd+1,&read_fds,NULL,NULL,NULL)<0){ perror("select"); exit(1); }

        if(FD_ISSET(sfd,&read_fds)){
            int cfd = accept(sfd,NULL,NULL);
            if(cfd>=0){
                for(i=0;i<MAX_CLIENTS;i++){ if(clients[i]<0){ clients[i]=cfd; break; } }
                if(cfd>maxfd) maxfd=cfd;
                FD_SET(cfd,&all_fds);
                printf("Client connected.\n");
            }
        }

        for(i=0;i<MAX_CLIENTS;i++){
            int fd = clients[i];
            if(fd<0) continue;
            if(FD_ISSET(fd,&read_fds)){
                ssize_t n = read(fd,buf,BUF_SIZE-1);
                if(n<=0){ close(fd); FD_CLR(fd,&all_fds); clients[i]=-1; printf("Client disconnected.\n"); }
                else{ write(fd,buf,n); buf[n]='\0'; printf("Echoed: %s", buf); }
            }
        }
    }
}
