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

void reverse(){
  while(tail != NULL){
    insert(&head, tail->task);
    delete(&tail, tail->task);
  }
}

void schedule(){
  reverse();

  struct node *next = head;
  while(head != NULL){
    int i = 0;
    if (next->task->burst <= 10){
      i += next->task->burst;
      run(next->task, i);
      if (next->next == NULL)
        next = head;
      else
        next = next->next;
      delete(&head, next->task);
    }
    else{
      i += 10;
      next->task->burst -= 10;
      run(next->task, i);
      if (next->next == NULL)
        next = head;
      else
        next = next->next;
    }
  }
}
