#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define MQ_NAME "/testMessageQueue"
#define MAX_MSG_SIZE 128

struct mq_attr mq_attr;
static char messageToSend[] = "****Welcome to Real Time Embedded System****";

void *senderFunc(void *vargp)
{
    printf("Entered sender function\n");
    mqd_t sendermq;
    
    sendermq = mq_open(MQ_NAME, O_CREAT|O_RDWR, S_IRWXU, &mq_attr);

    if(sendermq < 0)
    {
        perror("mq_open failed in sender");
        exit(-1);
    }
    else
    {
        printf("message queue opened by the sender\n");
    }

    /* send message with priority=30 */
    if((mq_send(sendermq, messageToSend, sizeof(messageToSend), 25)) == -1)
    {
        perror("mq_send failed");
    }
    else
    {
        printf("Message successfully sent from the sender\n");
    }
      
    return NULL;
}

void *receiverFunc(void *vargp)
{
    printf("Entered receiver function\n");
    mqd_t receivermq;
    char receivebuffer[MAX_MSG_SIZE];
    unsigned int prio;
    int nbytes;

    receivermq = mq_open(MQ_NAME, O_CREAT|O_RDWR, S_IRWXU, &mq_attr);

    if(receivermq == -1)
    {
        perror("mq_open and create in receiver failed");
        exit(-1);
    }

    if((nbytes = mq_receive(receivermq, receivebuffer, MAX_MSG_SIZE, &prio)) == -1)
    {
        perror("mq_receive failed");
    }
    else
    {
        receivebuffer[nbytes] = '\0';
        printf("Message Received : %s \npriority = %d, length = %d\n",
            receivebuffer, prio, nbytes);
    }
        return NULL;
}

int main()
{
    mq_attr.mq_maxmsg = 10;
    mq_attr.mq_msgsize = MAX_MSG_SIZE;
    mq_attr.mq_flags = 0;
    pthread_t sender_thread,receiver_thread;
    pthread_create(&sender_thread, NULL, senderFunc,NULL);
    pthread_create(&receiver_thread, NULL, receiverFunc,NULL);
    pthread_join(sender_thread,NULL);
    pthread_join(receiver_thread,NULL);
    return 0;
}
