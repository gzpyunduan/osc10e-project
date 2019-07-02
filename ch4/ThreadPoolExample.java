/**
 * Creating a Java thread pool.
 *
 * Figure 4.15
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts  - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

import java.util.concurrent.*;

public static sort([]array, int l, int r){
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

public static int partition(int []array, int lo, int hi){
  int key = array[lo];
  while (lo < hi){
    while (array[hi] >= key && hi > lo)
      hi--;
    array[lo] = array[hi];
    while (array[lo] <= key && hi > lo)
      lo++;
    array[hi] = array[lo]
  }
  array[hi] = key;
  return array[hi];
}

public class QuickSort extends RecursiveAction{
  private static final int Max = 100;

  private int lo;
  private int hi;
  private int []array;

  public QuickSort(int []array, int lo, int hi){
    this.array = array;
    this.lo = lo;
    this.array = array;
  }

  protected void qsort(){
    if (lo >= hi)
      return;
    int index = partition(array, lo, hi);
    if (index - 1 - lo >= Max){
      QuickSort left = new QuickSort(array, lo, index - 1);
      left.fork();
    }
    else
      sort(array, lo, index - 1);
    if (hi - index - 1 >= Max){
      QuickSort right = new QuickSort(array, index + 1, hi);
      right.fork();
    }
    else
      sort(array, index + 1, hi);
  }
}

class Task implements Runnable
{
    public void run() {
        System.out.println(Thread.currentThread().getName());
    }
}

public class ThreadPoolExample
{
    public static void main(String[] args) {
        int numTasks = Integer.parseInt(args[0].trim());

        ExecutorService pool = Executors.newCachedThreadPool();

        for (int i = 0; i < numTasks; i++)
            pool.execute(new Task());

            // The following invovles using a lambda function
            // pool.execute( () -> {System.out.println(Thread.currentThread().getName());});

        pool.shutdown();
    }
}
