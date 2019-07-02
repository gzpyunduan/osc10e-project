#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int *list;
int *ans;
int length;

void sort(int l, int r, int *a);
void *sort1(void *param);
void *sort2(void *param);
void *merge(void *param);

int main(int argc, char *argv[])
{
  pthread_t tid1, tid2, tid3; /* the thread identifier */
  pthread_attr_t attr1, attr2, attr3; /* set of attributes for the thread */

  length = argc - 1;

  list = (int *)calloc(length, sizeof(int));
  ans = (int *)calloc(length, sizeof(int));

  for (int i = 0; i < length; i++)
    list[i] = atoi(argv[i + 1]);

  pthread_attr_init(&attr1);
  pthread_attr_init(&attr2);
  pthread_create(&tid1, &attr1, sort1, NULL);
  pthread_create(&tid2, &attr2, sort2, NULL);
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  pthread_attr_init(&attr3);
  pthread_create(&tid3, &attr3, merge, NULL);
  pthread_join(tid3, NULL);

  for (int i = 0; i < length; i++)
    printf("%d ", ans[i]);
}

void sort(int l, int r, int *a){
  int v;
  for (int i = l; i < r; i++){
    for (int j = i + 1; j <= r; j++){
      if(a[i] > a[j]){
        v = a[i];
        a[i] = a[j];
        a[j] = v;
      }
    }
  }
}

/**
 * The thread will begin control in this function
 */
void *sort1(void *param){
  sort(0, length / 2 - 1, list);

  pthread_exit(0);
}

void *sort2(void *param){
  sort(length / 2, length - 1, list);

  pthread_exit(0);
}

void *merge(void *param){
  int i = 0;
  int j = length / 2;
  int k = 0;

  while(1){
    if(list[i] < list[j])
      ans[k++] = list[i++];
    else
      ans[k++] = list[j++];

    if(i == length / 2){
      for (int l = j; l < length; l++)
        ans[k++] = list[l];
      break;
    }
    else if (j == length){
      for (int l = i; l < length / 2; l++)
        ans[k++] = list[l];
      break;
    }
  }

  pthread_exit(0);
}
