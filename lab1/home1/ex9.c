#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "message_queue.h"

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main() {
    int msqid;
    struct msqid_ds buf;

    msqid = msgget(IPC_PRIVATE, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (msqid == -1) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }
    printf("Created new message queue. ID = %d\n", msqid);

    if (msgctl(msqid, IPC_STAT, &buf) == -1) {
        perror("msgctl IPC_STAT failed");
        exit(EXIT_FAILURE);
    }

    buf.msg_qbytes = 5;
    if (msgctl(msqid, IPC_SET, &buf) == -1) {
        perror("msgctl IPC_SET failed");
        exit(EXIT_FAILURE);
    }
    printf("Set msg_qbytes = 5\n");

    struct msgbuf message;
    message.mtype = 1;
    strcpy(message.mtext, "Test message");  

    if (msgsnd(msqid, &message, strlen(message.mtext)+1, 0) == -1) {
        perror("msgsnd failed");
    } else {
        printf("Message sent successfully\n");
    }

    exit(EXIT_SUCCESS);
}
