public class Quicksort {

	private static native void println(String s);

	private static native void println(int n);

	public static void main(String[] args) {
		int[] array = { 12, 3, 5, 1, 7, 9, 21, 5, 6, 4};

		println("Before sorting");
		println(array);
		sort(array);
		println("After sorting");
		println(array);
	}

	private static void sort(int[] array) {
		quicksort(array, 0, array.length - 1);	
	}

	private static void quicksort(int[] array, int low, int high) {
		int l = low;
		int h = high;
		int pivot = array[low + (high - low) / 2];

		while (l <= h) {
			while (array[l] < pivot) {
				l++;
			}
			while (array[h] > pivot) {
				h--;
			}

			if (l <= h) {
				int tmp = array[l];
				array[l] = array[h];
				array[h] = tmp;
				l++;
				h--;
			}
		}
		if (low < h) {
			quicksort(array, low, h);
		}
		if (l < high) {
			quicksort(array, l, high);
		}
	}

	private static void println(int[] array) {
		for (int i = 0; i < array.length; i++) {
			println(array[i]);
		}
	}
}
