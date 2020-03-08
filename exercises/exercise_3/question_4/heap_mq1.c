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

#define SNDRCV_MQ "/send_receive_mq"
#define ERROR -1
struct mq_attr mq_attr;
mqd_t mymq;


/* receives pointer to heap, reads it, and deallocate heap memory */

void receiver(void)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr; 
  int prio;
  int nbytes;
  int count = 0;
  int id;
 
  while(1) {

    /* read oldest, highest priority msg from the message queue */

    printf("Reading %ld bytes\n", sizeof(void *));
  
    if((nbytes = mq_receive(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), &prio)) == ERROR)

    {
      perror("mq_receive");
    }
    else
    {
      memcpy(&buffptr, buffer, sizeof(void *));
      memcpy((void *)&id, &(buffer[sizeof(void *)]), sizeof(int));

      printf("contents of ptr = \n%s\n", (char *)buffptr);

      free(buffptr);

      printf("heap space memory freed\n");

    }
    
  }

}


static char imagebuff[4096];

void sender(void)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr;
  int prio;
  int nbytes;
  int id = 999;


  while(1) {

    /* send malloc'd message with priority=30 */

    buffptr = (void *)malloc(sizeof(imagebuff));
    strcpy(buffptr, imagebuff);
    printf("Message to send = %s\n", (char *)buffptr);

    printf("Sending %ld bytes\n", sizeof(buffptr));

    memcpy(buffer, &buffptr, sizeof(void *));
    memcpy(&(buffer[sizeof(void *)]), (void *)&id, sizeof(int));

    if((nbytes = mq_send(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), 30)) == ERROR)
    {
      perror("mq_send");
    }

    sleep(3);

  }
  
}


static int sid, rid;

void main(void)
{
  int i, j;
  char pixel = 'A';

  for(i=0;i<4096;i+=64) {
    pixel = 'A';
    for(j=i;j<i+64;j++) {
      imagebuff[j] = (char)pixel++;
    }
    imagebuff[j-1] = '\n';
  }
  imagebuff[4095] = '\0';
  imagebuff[63] = '\0';

  printf("buffer =\n%s", imagebuff);

  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = sizeof(void *)+sizeof(int);
  mq_attr.mq_flags = 0;
  mq_attr.mq_curmsgs =0;

  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0664, &mq_attr);

  if(mymq == (mqd_t)ERROR)
    perror("mq_open");

    pthread_t receiver_thread, sender_thread;
  
  if(pthread_create(&receiver_thread, NULL,(void *)receiver, NULL)==0)
	printf("\n\rReceiver Thread Created Sucessfully!\n\r");
  else perror("thread creation failed");

  if(pthread_create(&sender_thread, NULL ,(void *) sender, NULL)==0)
	printf("\n\rSender Thread Created Sucessfully!\n\r");  
  else perror("thread creation failed");

  pthread_join(receiver_thread, NULL);
  pthread_join(sender_thread, NULL);
  
  mq_close(mymq);
  if(mymq == (mqd_t)ERROR)
    perror("mq_close");


}

