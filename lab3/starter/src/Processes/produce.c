#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>

int main (int argc, char *argv[]) {
	int N, P, B, id;

	N = atoi(argv[1]);
	B = atoi(argv[2]);
	P = atoi(argv[3]);
	id = atoi(argv[4]);

  	//struct mq_attr output_queue_attr;

	mqd_t qdes;
	char *qname = "/m5mathew_mailbox";
/*	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;

	attr.mq_maxmsg  = B;
	attr.mq_msgsize = sizeof( int );
	attr.mq_flags   = 0		*/

	qdes  = mq_open(qname, O_RDWR);
	if (qdes == -1 ) {
		perror("mq_open() failed");
		exit(1);
	}
	
	mqd_t qdes_cs;
	char *qname_cs = "/m5mathew_mailbox_cs";
	
	qdes_cs = mq_open(qname_cs, O_RDWR);
	if (qdes_cs == -1) {
		perror("mq_open() for consumption failed");
		exit(1);
	}



	int i = id;

	while(i < N) {
		if(mq_send(qdes, (char*)&i, sizeof(int), 0) == -1){
			perror("mq_send() failed");
		}

		i += P;
	}
	
	char *num_prod_cons;
	mq_receive(qdes_cs, num_prod_cons, sizeof(int), 0);

	int producers = atoi(strtok(num_prod_cons, " "));
	producers--;
	int consumers = atoi(strtok(NULL, " "));
	sprintf(num_prod_cons, "%d %d", producers, consumers);


	if(mq_send(qdes_cs, num_prod_cons, sizeof(int), 0) == 
-1){
		perror("mq_send() failed");
	}

	
	
/*
	if (mq_close(qdes) == -1){
		perror("mq_close() failed");
		exit(2);
	}

	if (mq_unlink(qname) != 0) {
		perror("mq_unlink() failed");
		exit(3);
	}
*/
	exit(0);

}
