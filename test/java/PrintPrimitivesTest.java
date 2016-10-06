public class PrintPrimitivesTest {

	private static native void println(String s);
	private static native void println(char c);
	private static native void println(byte b);
	private static native void println(short s);
	private static native void println(int n);
	private static native void println(long n);
	private static native void println(float x);
	private static native void println(double x);
	private static native void println(boolean b);

	public static void main(String args[]) {
		println("String");
		println('c');
		byte b = 127;
		println(b);
		short s = 32767;
		println(s);
		println(2147483647);
		println(2147483648L);
		println(10.123456f);
		println(100.123456);
		println(true);
		println(false);
	}
}
