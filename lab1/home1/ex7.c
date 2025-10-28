#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "message_queue.h"

int main()
{
    int msqid;

    msqid = msgget(IPC_PRIVATE, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (msqid == -1) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }

    printf("New message queue created. ID = %d\n", msqid);

    exit(EXIT_SUCCESS);
}
