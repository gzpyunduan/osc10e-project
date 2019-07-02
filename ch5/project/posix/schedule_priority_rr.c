#include <stdio.h>
#include <stdlib.h>
#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node *tail;
struct node *head;

void add(char *name, int priority, int burst){
  Task *task = (Task*)malloc(sizeof(Task));
  task->name = name;
  task->priority = priority;
  task->burst = burst;

  insert(&tail, task);
}

void sort(){
  while(tail != NULL){
    struct node *next = tail->next;
    int max = tail->task->priority;
    Task *mTask = tail->task;
    while(next != NULL){
      if(next->task->priority > max){
        max = next->task->priority;
        mTask = next->task;
      }
      next = next->next;
    }
    insert(&head, mTask);
    delete(&tail, mTask);
  }
}

void schedule(){
  sort();

  int slice = 0;
  while(head != NULL){
    int pri = head->task->priority;
    if(head->next == NULL || head->next->task->priority != pri){
      slice += head->task->burst;
      run(head->task, slice);
      delete(&head, head->task);
    }
    else{
      struct node *next = head;
      while(head->task->priority == pri){
        if(next->task->burst <= 10){
          slice += next->task->burst;
          run(next->task, slice);
          delete(&head, next->task);
        }
        else{
          slice += 10;
          run(next->task, slice);
          next->task->burst -= 10;
        }

        if(next->next->task->priority != pri)
          next = head;
        else
          next = next->next;
      }
    }
  }
}
