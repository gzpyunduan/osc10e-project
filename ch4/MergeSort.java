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

public class MergeSort extends RecursiveAction{
  private int lo;
  private int hi;
  private int mid;
  private int []array;

  public MergeSort(int []array, int lo, int hi){
    this.array = array;
    this.lo = lo;
    this.hi = hi;
    this.mid = (lo + hi) / 2;
  }

  public void merge(){
    int []tmp = new int[hi - lo + 1];
    int p1 = lo, p2 = mid + 1, k = 0;

    while(p1 <= mid && p2 <= hi){
      if(array[p1] <= array[p2])
        tmp[k++] = array[p1++];
      else
        tmp[k++] = array[p2++];
    }

    while(p1 <= mid) tmp[k++] = array[p1++];
    while(p2 <= hi) tmp[k++] = array[p2++];

    for (int i = lo; i <= hi; i++)
      array[i] = tmp[i - lo];
  }

  protected void compute(){
    if (hi <= lo)
      return;

    MergeSort left = new MergeSort(array, lo, mid);
    MergeSort right = new MergeSort(array, mid + 1, hi);

    left.fork();
    right.fork();
    left.join();
    right.join();

    merge();
  }

	public static void main(String[] args) {
		ForkJoinPool pool = new ForkJoinPool();
    int SIZE = 100;
		int []array = new int[SIZE];

		// create SIZE random integers between 0 and 9
		java.util.Random rand = new java.util.Random();

		for (int i = 0; i < SIZE; i++) {
			array[i] = rand.nextInt(100);
		}

    for (int i = 0; i < SIZE; i++)
      System.out.print(array[i] + " ");

		// use fork-join parallelism to sum the array
		MergeSort task = new MergeSort(array, 0, SIZE - 1);

    pool.invoke(task);

		for (int i = 0; i < SIZE; i++)
      System.out.println(array[i]);
	}
}
