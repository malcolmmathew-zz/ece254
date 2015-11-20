#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include "common.h"
#include <signal.h>
#include <stdbool.h>
#include <math.h>

int main (int argc, char *argv[]) {
	int N, P, B, id;

	N = atoi(argv[1]);
	B = atoi(argv[2]);
	P = atoi(argv[3]);
	id = atoi(argv[4]);


//  	struct mq_attr output_queue_attr;

	mqd_t qdes;
	char *qname = "/m5mathew_mailbox";
/*	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;

	attr.mq_maxmsg  = B;
	attr.mq_msgsize = sizeof( int );
	attr.mq_flags   = 0;		*/
	
	qdes  = mq_open(qname, O_RDWR | O_NONBLOCK);
	if (qdes == (mqd_t)-1 ) {
		perror("mq_open() consumer reg queue failed");
		exit(1);
	}

//	mq_getattr(qdes, &output_queue_attr);

	mqd_t qdes_cs;
	char *qname_cs = "/m5mathew_mailbox_cs";
/*	mode_t mode_cs = S_IRUSR | S_IWUSR;
	struct mq_attr attr_cs;

	attr_cs.mq_maxmsg = 1;
	attr_cs.mq_msgsize = sizeof( int );
	attr_cs.mq_flags = 0;
*/
	qdes_cs = mq_open(qname_cs, O_RDWR);
	if (qdes_cs == -1){
		perror("mq_open() for consumption failed");
		exit(1);
	}

	char num_prod_cons[5];
	int producers, consumers, message, root_val;
	while(1){
		if( mq_receive(qdes, (char *)&message, sizeof(int), 0) == -1){
			if( mq_receive(qdes_cs, num_prod_cons, sizeof(int), 0) == -1)
			{
				perror("qdes_cs empty");
				fflush(stdout);
			}
			else {
				producers = atoi(strtok(num_prod_cons, " "));
				consumers = atoi(strtok(NULL, " "));
				if (producers == 0) {
					consumers--;
					sprintf(num_prod_cons, " %d %d ", producers, consumers);
			
					if(mq_send(qdes_cs, num_prod_cons, sizeof(int), 0) == -1){
						perror("mq_send() failed");
					}
					break;
				}
				else {
					sprintf(num_prod_cons, " %d %d ", producers, consumers);				
					if(mq_send(qdes_cs, num_prod_cons, sizeof(int), 0) == -1){

					perror("mq_send() failed");
					}				
				}
			}
		}
		
		root_val = sqrt(message);
		if (root_val * root_val == message) {
			printf("%i %i %i\n", id, message, root_val);
		}
		

	}

	if (consumers == 0) {
		printf("System execution time: %d", time_elapsed);
		if (mq_close(qdes) == -1) {
			perror("mq_close() failed");
			exit(2);
		}
	
		if (mq_unlink(qname) != 0) {
			perror("mq_unlink() failed");
			exit(3);
		}
		if(mq_close(qdes_cs) == -1){
			perror("mq_close() failed");
			exit(2);
		}
		if(mq_unlink(qname_cs) == -1){
			perror("mq_unlink() failed");
			exit(3);
		}
	}

	exit(0);
	
}
