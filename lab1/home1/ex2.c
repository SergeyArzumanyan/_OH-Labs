#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>
#include "message_queue.h"

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main() {
    int msqid;
    struct msgbuf message;

    msqid = msgget(MSQ_ID, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (msqid == -1) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }

    message.mtype = 20;
    strcpy(message.mtext, "test");

    if (msgsnd(msqid, &message, sizeof(message.mtext), 0) == -1) {
        perror("msgsnd failed");
        exit(EXIT_FAILURE);
    }

    printf("Message sent successfully: mtype=%ld, mtext=%s\n", message.mtype, message.mtext);

    return 0;
}
