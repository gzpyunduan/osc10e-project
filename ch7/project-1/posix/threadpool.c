/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be
// completed by a thread in the pool
typedef struct
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task worktodo;

sem_t sem;

pthread_mutex_t mutex;

// the worker bee
pthread_t bee[3];
//int available[3];

task queue[QUEUE_SIZE];

int tmpsize=0;
int head=0;
int tail=0;
// insert a task into the queue
// returns 0 if successful or 1 otherwise,
int enqueue(task t)
{
    if(tmpsize<QUEUE_SIZE)
    {
	//printf("enqueue: %d  head is %d tail is %d \n",tmpsize , head,tail);
	queue[tail].function=t.function;
	queue[tail].data=t.data;
	tail++;
 	tail=tail%QUEUE_SIZE;
	tmpsize++;
	sem_post(&sem);
    	return 0;
    }
    else 	return 1;
}

// remove a task from the queue
task dequeue()
{
    if(tmpsize>0)
    {
	//printf(" %d  head is %d tail is %d \n",tmpsize , head,tail);
	    worktodo.function=queue[head].function;
	    worktodo.data=queue[head].data;
	    head++;
	    head=head%QUEUE_SIZE;
	    tmpsize--;
    }

    return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
	task T;

  while(1){
	  sem_wait(&sem);
    pthread_mutex_lock(&mutex);
    T = dequeue();
    pthread_mutex_unlock(&mutex);
    execute(T.function, T.data);
  }

  pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
	//printf("something useful\n");
    (*somefunction)(p);

}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    //int j=0;
    worktodo.function = somefunction;
    worktodo.data = p;

    pthread_mutex_lock(&mutex);
    enqueue(worktodo);
    pthread_mutex_unlock(&mutex);

    return 0;
}

// initialize the thread pool
void pool_init(void)
{
  pthread_mutex_init(&mutex,NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
  sem_init(&sem,0,0);
  //for(j=0;j<3;j++)
  //{
	if(0 != pthread_create(&bee[0],NULL,(void*)worker,NULL))	printf("error");
	if(0 != pthread_create(&bee[1],NULL,(void*)worker,NULL))	printf("error");
	if(0 != pthread_create(&bee[2],NULL,(void*)worker,NULL))	printf("error");
	//pthread_create(&bee[2],NULL,(void*)worker,NULL);
	//available[j]=1;

    //}

}

// shutdown the thread pool
void pool_shutdown(void)
{
    int j = 0;

    for(j = 0; j < 3; j++)
    {
	    pthread_cancel(bee[j]);
    	pthread_join(bee[j], NULL);
    }
}
