#include <stdio.h>
#include <stdlib.h>
#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node *tail;

void add(char *name, int priority, int burst){
  Task *task = (Task*)malloc(sizeof(Task));
  task->name = name;
  task->priority = priority;
  task->burst = burst;

  insert(&tail, task);
}

void schedule(){
  int i = 0;
  while(tail != NULL){
    int min = tail->task->burst;
    Task *mTask = tail->task;
    struct node *next = tail->next;
    while(next != NULL){
      if (next->task->burst < min){
        min = next->task->burst;
        mTask = next->task;
      }
      next = next->next;
    }
    i += min;
    run(mTask, i);
    delete(&tail, mTask);
  }
}
