#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

typedef int buffer_item;
#define BUFFER_SIZE 5

buffer_item buffer[BUFFER_SIZE];
int sleepTime;
int npt;
int nct;

sem_t available;
sem_t number;
pthread_mutex_t mutex;

int n=0;
int headindex=0;
int tailindex=0;

int insert_item(buffer_item item);
int remove_item(buffer_item *item);
int bufferInit();
void *producer(void *param);
void *consumer(void *param);

int main(int argc, char **argv)
{
	int counter=0;

	bufferInit();
	sleepTime=atoi(argv[1]);
	npt=atoi(argv[2]);
	nct=atoi(argv[3]);

	pthread_t ptid[npt];
	pthread_t ctid[nct];

	for(;counter<npt;counter++)
	{
		pthread_create(&ptid[counter], NULL, producer, NULL);
	}
	for(counter=0;counter<nct;counter++)
	{
		pthread_create(&ctid[counter], NULL, consumer, NULL);
	}
	sleep(sleepTime);

	for(counter=0;counter<npt;counter++)
	{
		pthread_cancel(ptid[counter]);
		pthread_join(ptid[counter], NULL);
	}

	for(counter=0;counter<nct;counter++)
	{
		pthread_cancel(ctid[counter]);
		pthread_join(ctid[counter], NULL);
	}

	return 0;
	//printf("%d,%d,%d \n",sleepTime,npt,nct);
}

int bufferInit()
{
	srand(time(NULL));
	pthread_mutex_init(&mutex,NULL);
	sem_init(&available,0,BUFFER_SIZE);
	sem_init(&number,0,0);
}


int insert_item(buffer_item item)
{
	sem_wait(&available);
	pthread_mutex_lock(&mutex);
	//if(n==BUFFER_SIZE)
	//	return -1;
	buffer[tailindex]=item;
	tailindex++;
	tailindex=tailindex%BUFFER_SIZE;
	n++;
	pthread_mutex_unlock(&mutex);
	sem_post(&number);
	return 0;
}

int remove_item(buffer_item *item)
{
	sem_wait(&number);
	pthread_mutex_lock(&mutex);
	//if(n==0)
	//	return -1;
	*item=buffer[headindex];
	headindex++;
	headindex=headindex%BUFFER_SIZE;
	n--;

	pthread_mutex_unlock(&mutex);
	sem_post(&available);
	return 0;
}

void *producer(void *param)
{
	int randSleepTime;
	buffer_item item;

	while(1)
	{
		randSleepTime=rand()*3/RAND_MAX+1;
		sleep(randSleepTime);//change to a random int
		item=rand();
		//pthread_mutex_lock(&mutex);
		if(insert_item(item))
			printf("error in insert\n");
		else
			printf("producer produced %d     %d\n",item,n);
		//pthread_mutex_unlock(&mutex);

	}
}
void *consumer(void *param)
{
	int randSleepTime;
	buffer_item item;
	while(1)
	{
		randSleepTime=rand()*3/RAND_MAX+1;
		sleep(randSleepTime);//random rand()*4/RAND_MAX

		if(remove_item(&item))
			printf("error in remove\n");
		else
			printf("consumer consumed %d     %d\n",item,n);

	}
}
