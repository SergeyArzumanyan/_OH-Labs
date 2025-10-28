#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include "message_queue.h"

struct msgbuf
{
    long mtype;
    char mtext[100];
};

int main()
{
    int msqid;
    struct msgbuf message;

msqid = msgget(MSQ_ID, 0);
if (msqid == -1)
{
    perror("msgget failed");
    exit(EXIT_FAILURE);
}

printf("Reading all messages from the queue...\n");

while (msgrcv(msqid, &message, sizeof(message.mtext), 0, IPC_NOWAIT) != -1)
{
    printf("Received message: mtype=%ld, mtext=%s\n", message.mtype, message.mtext);
}

perror("No more messages in the queue");

return 0;
}
