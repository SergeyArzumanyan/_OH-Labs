#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include "message_queue.h"

int main() {
    int msqid = init_queue();
    struct mbuf msg;
    ssize_t msgLen;

    printf("Reading all messages from queue (msqid=%d)...\n", msqid);

    // Keep reading until there are no more messages
    while (1) {
        msgLen = msgrcv(msqid, &msg, MAX_MTEXT, 0, IPC_NOWAIT);

        if (msgLen == -1) {
            if (errno == ENOMSG) {
                printf("Queue is now empty.\n");
                break;
            } else {
                perror("msgrcv error");
                exit(EXIT_FAILURE);
            }
        }

        printf("Received message: type=%ld, text=%s\n", msg.mtype, msg.mtext);
    }


    printf("\nTrying to read once more from the empty queue...\n");
    msgLen = msgrcv(msqid, &msg, MAX_MTEXT, 0, IPC_NOWAIT);

    if (msgLen == -1 && errno == ENOMSG) {
        printf("No messages available. msgrcv() returned -1 with ENOMSG.\n");
    } else if (msgLen >= 0) {
        printf("Unexpectedly received: %s\n", msg.mtext);
    } else {
        perror("msgrcv error");
    }

    return 0;
}

