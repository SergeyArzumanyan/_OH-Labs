#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include "message_queue.h"

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main() {
    int msqid;
    struct msgbuf message;

    msqid = msgget(MSQ_ID, 0);
    if (msqid == -1) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening for messages... (press Ctrl+C to stop)\n");

    while (1) {
        if (msgrcv(msqid, &message, sizeof(message.mtext), 0, 0) == -1) {
            perror("msgrcv failed");
            exit(EXIT_FAILURE);
        }
        printf("Received message: mtype=%ld, mtext=%s\n", message.mtype, message.mtext);
    }

    return 0;
}
