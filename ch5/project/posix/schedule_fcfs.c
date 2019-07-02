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
  int i = 0;
  while(head != NULL){
    i += head->task->burst;
    run(head->task, i);
    delete(&head, head->task);
  }
}
