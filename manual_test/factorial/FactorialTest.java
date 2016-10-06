public class FactorialTest {

	private static native void println(int i);

	public static int factorial(int n) {
		if (n <= 1) {
			return 1;
		} else {
			return n * factorial(n - 1);
		}
	}

	public static void main(String args[]) {
		println(factorial(10));
	}
}
