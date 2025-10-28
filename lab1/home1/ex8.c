#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "message_queue.h"

int main(int argc, char *argv[])
{
    if (argc < 2 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s [msqid...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int j = 1; j < argc; j++) {
        int msqid = atoi(argv[j]);
        if (msgctl(msqid, IPC_RMID, NULL) == -1) {
            fprintf(stderr, "Failed to remove queue %d: %s\n", msqid, strerror(errno));
        } else {
            printf("Message queue %d removed successfully.\n", msqid);
        }
    }

    exit(EXIT_SUCCESS);
}
