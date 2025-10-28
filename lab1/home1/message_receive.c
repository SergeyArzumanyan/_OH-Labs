#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include "message_queue.h"

int main()
{
    int msqid = init_queue();
    int type = 20;              // read messages of type 20
    size_t maxBytes = 5;        // deliberately small buffer
    ssize_t msgLen;
    struct mbuf msg;

    // Use both IPC_NOWAIT (non-blocking) and MSG_NOERROR (truncate long messages)
    msgLen = msgrcv(msqid, &msg, maxBytes, type, IPC_NOWAIT | MSG_NOERROR);

    if (msgLen == -1) {
        if (errno == ENOMSG) {
            printf("No messages in queue — non-blocking mode, exiting.\n");
            exit(EXIT_SUCCESS);
        } else {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
    }

    printf("Received (possibly truncated): type=%ld; length=%ld; body=%s\n",
           msg.mtype, (long) msgLen, msg.mtext);
    return 0;
}

