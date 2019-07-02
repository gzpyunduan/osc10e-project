/**
 * Fork/join parallelism in Java
 *
 * Figure 4.18
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts  - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

import java.util.concurrent.*;

public class QuickSort extends RecursiveAction{
  static final int Max = 100;
  static final int SIZE = 1000;

  private int lo;
  private int hi;
  private int []array;

  public QuickSort(int []array, int lo, int hi){
    this.array = array;
    this.lo = lo;
    this.hi = hi;
  }

  protected void compute(){
    if (hi - lo < Max){
      int v;
      for (int i = lo; i < hi; i++){
        for (int j = i + 1; j <= hi; j++){
          if(array[i] > array[j]){
            v = array[i];
            array[i] = array[j];
            array[j] = v;
          }
        }
      }
      return;
    }

    int key = array[lo];
    int l = lo;
    int h = hi;
    while (l < h){
      while (array[h] >= key && h > l)
        h--;
      array[l] = array[h];
      while (array[l] <= key && h > l)
        l++;
      array[h] = array[l];
    }
    array[h] = key;

    QuickSort left = new QuickSort(array, lo, h - 1);
    QuickSort right = new QuickSort(array, h + 1, hi);

    left.fork();
    right.fork();
  }

	public static void main(String[] args) {
		ForkJoinPool pool = new ForkJoinPool();
		int []array = new int[SIZE];

		// create SIZE random integers between 0 and 9
		java.util.Random rand = new java.util.Random();

		for (int i = 0; i < SIZE; i++) {
			array[i] = rand.nextInt(10000);
		}

		// use fork-join parallelism to sum the array
		QuickSort task = new QuickSort(array, 0, SIZE - 1);

    pool.invoke(task);

		for (int i = 0; i < SIZE; i++)
      System.out.println(array[i]);
	}
}
