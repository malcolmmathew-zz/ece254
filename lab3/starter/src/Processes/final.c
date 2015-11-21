/***********************************************************************
* Code listing from "Advanced Linux Programming," by CodeSourcery LLC  *
* Copyright (C) 2001 by New Riders Publishing                          *
* See COPYRIGHT for license information.                               *
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>


/* Spawn a child process running a new program.  PROGRAM is the name
   of the program to run; the path will be searched for this program.
   ARG_LIST is a NULL-terminated list of character strings to be
   passed as the program's argument list.  Returns the process id of
   the spawned process.  */

int spawn (char* program, char** arg_list, int id, double t1)
{
	arg_list[0] = program;

	char id_str[3];
	sprintf(id_str, "%d", id);
	

	char start_time[17];
	sprintf(start_time, "%f", t1);	

	arg_list[2] = start_time;
	arg_list[4] = id_str;

	/* Duplicate this process.  */
	pid_t child_pid;
	child_pid = fork ();

	if (child_pid != 0)
		/* This is the parent process.  */
		return child_pid;
	else {
		/* Now execute PROGRAM, searching for it in the path.  */
		execvp (program, arg_list);
		/* The execvp function returns only if an error occurs.  */
		fprintf (stderr, "an error occurred in execvp\n");
		abort ();
	}
}

int main ( int argc, char *argv[] )
{
	if (argc < 5) {
		printf("Incorrect number of arguments. \n");
		exit(1);
	}
	
	pid_t wpid;	
	int status;

	struct timeval tv;
	double t1;

	int N = atoi(argv[1]);
	int B = atoi(argv[2]);
	int P = atoi(argv[3]);
	int C = atoi(argv[4]);


	if(N < 0 || B < 0 || P < 0 || C < 0) {
		printf("Not all arguments are positive integers. \n");
		exit(1);
	}

	
//	struct mq_attr output_queue_attr;
	
	mqd_t qdes;
	char *qname = "/m5mathew_mailbox";
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;

	attr.mq_maxmsg  = B;
	attr.mq_msgsize = sizeof( int );
	attr.mq_flags   = 0;

	qdes  = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
	if (qdes == -1 ) {
		perror("mq_open() process has failed!!!");
		exit(1);
	}

//	mq_getattr(qdes_cs, &output_queue_attr);

	
	mqd_t qdes_cs;
	char *qname_cs = "/m5mathew_mailbox_cs";
	mode_t mode_cs = S_IRUSR | S_IWUSR;
	struct mq_attr attr_cs;


	attr_cs.mq_maxmsg = 1;
	attr_cs.mq_msgsize = sizeof( int );
	attr_cs.mq_flags = 0;		/* a blocking queue  */
	
	
	qdes_cs = mq_open(qname_cs, O_RDWR | O_CREAT, mode_cs, &attr_cs);
	if (qdes_cs == -1){
		perror("mq_open() for consumption failed");
		exit(1);
	}
	
	char *num_prod_cons;
	sprintf(num_prod_cons, " %d %d ", P, C);
	
	if(mq_send(qdes_cs, num_prod_cons, sizeof( int ), 0) != 0)
	{
		perror("mq_send() failed");
		exit(1);
	}


	int i, j;

	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + tv.tv_usec/1000000.0;
	
	for(i = 0; i < P; i++) {
		spawn("./produce", argv, i, 0.0);
	}

	for(j = 0; j < C; j++) {
		spawn("./consume", argv, j, t1);
	}


//	while((wpid = wait(&status)) > 0);
/*	int consume_val;
	while(1){
	mq_receive(qdes_cs, (char *)&consume_val, sizeof(int), 0);
	if(consume_val == 0) {
		break;
	}

	}
*/
/*
	if (mq_close(qdes) == -1){
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
		perror("mq_unlink() failed ");
		exit(3);
	}
*/

  	return 0;
}


