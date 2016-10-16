public class ArrayTest {

	private static native void println(int n);

	public static void main(String args[]) {
		int arg[] = { 1, 2, 3 };
		for (int i = 0; i < arg.length; i++) {
			println(arg[i]);
		}
	}
}
