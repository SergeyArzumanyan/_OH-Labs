#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include "message_queue.h"

int main()
{
    int msqid = init_queue(); 
    struct mbuf msg;

    msg.mtype = 20;
    strcpy(msg.mtext, "test");

    if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    printf("Message sent successfully: type=%ld, text=%s\n", msg.mtype, msg.mtext);
    return 0;
}

