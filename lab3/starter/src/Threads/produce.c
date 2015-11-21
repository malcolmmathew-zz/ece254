/***********************************************************************
* Code listing from "Advanced Linux Programming," by CodeSourcery LLC  *
* Copyright (C) 2001 by New Riders Publishing                          *
* See COPYRIGHT for license information.                               *
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <semaphore.h>
#include <math.h>

/* Semaphores */
pthread_mutex_t mutex_buffer; // semaphore for critical section of adding or to buffer
sem_t bufferFilledCount; // semaphore for number of items filled in buffer waiting to be read
sem_t bufferAvailableCount; // semaphore for number of available spaces in buffer
volatile int N; // globally keeps track of max number of integers
volatile int B; // globablly keeps track of buffer size
volatile int P; // globally keeps track of number of producers
volatile int C; // globally keeps track of number of consumers
volatile int bufferCounter; // keeps track of size of buffer
volatile int itemsConsumed; // keeps track of number of items consumed
int *buffer; // array of buffer
int itemCounter = 0;

/** Producer Thread function **/
void *producer(void *arg) {
	int pid;
	int i;

	pid = *((int *)arg);

	/** produce item **/
	for(i = 0; i < N; i++) {
		if (i % P == pid) {
			sem_wait(&bufferAvailableCount); // decrease available buffer semaphore and ensure there's space to add
			pthread_mutex_lock(&mutex_buffer); // entering critical section..
			
			/** put item in buffer **/
			buffer[++bufferCounter] = i;
			
			pthread_mutex_unlock(&mutex_buffer); // leaving critical section..
			sem_post(&bufferFilledCount); // increase buffer filled that are available to be read
		}
	}
	
	free(arg);
	pthread_exit(NULL);
}

/** Consumer Thread function **/
void *consumer(void *arg) {
	int cid;
	int val;
	int sqrtVal;
	int i;

	cid = ((int *)arg)[0];

	while(itemsConsumed < N) {
		sem_wait(&bufferFilledCount); // ensure there's items in the buffer
		pthread_mutex_lock(&mutex_buffer);
		/** remove item from buffer **/
		val = buffer[bufferCounter--];
		itemsConsumed++;
		pthread_mutex_unlock(&mutex_buffer);
		sem_post(&bufferAvailableCount);
		/** consume item from buffer **/
		sqrtVal = sqrt(val);
		if (sqrtVal*sqrtVal == val) {
			// we have ourselves a perfect square
			printf("%d %d %d \n", cid, val, sqrtVal);
		}
	}

	free(arg);
	pthread_exit(NULL);
}

int main ( int argc, char *argv[] )
{
	int i;
	int *tmp;
	N = atoi(argv[1]);
	B = atoi(argv[2]);
	P = atoi(argv[3]);
	C = atoi(argv[4]);
	struct timeval tv;
	double t1;
	double t2;

	itemsConsumed = 0;
	bufferCounter = 0;
	buffer = (int *) malloc(sizeof(int)*B);

	pthread_t threads[P+C]; // we will have a total of P + C pthreads
	pthread_mutex_init(&mutex_buffer, NULL); // used for critical section 1
	sem_init(&bufferFilledCount, 0, 0); // initialize bufferFilled as being 0 (empty)
	sem_init(&bufferAvailableCount, 0, B - 1); // initialize bufferAvailable as being max size of buffer (B)

	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + tv.tv_usec/1000000.0;
	// create all producer threads
	for (i = 0; i < P;i++) {
		tmp = malloc(sizeof(int));
		*tmp = i;

		pthread_create(&threads[i], NULL, &producer, tmp);
	}
	// create all consumer threads
	for (i = 0; i < C;i++) {
		tmp = malloc(sizeof(int));
		*tmp = i;

		pthread_create(&threads[P + i], NULL, &consumer, tmp);
	}

	while(itemsConsumed < N);

	gettimeofday(&tv, NULL);
	t2 = tv.tv_sec + tv.tv_usec/1000000.0;

	printf("System execution time: %f seconds", t2-t1);
	
	free(buffer);
  	return 0;
}