#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "unix_stream.h"


#define BACKLOG 5

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int sfd, cfd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        perror("remove");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("SERVER STARTED. Waiting for clients...\n");

    for (;;) {
        cfd = accept(sfd, NULL, NULL);

        if (cfd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Client connected.\n");

        while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
            write(STDOUT_FILENO, buf, numRead);
        }

        if (numRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        close(cfd);
        printf("\nClient disconnected.\n");
    }
}





