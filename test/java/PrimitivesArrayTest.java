public class PrimitivesArrayTest {

	private static native void println(long n);

	private static native void println(float f);

	private static native void println(double d);

	private static native void println(byte b);

	private static native void println(char c);

	private static native void println(short s);

	private static char[] charArray;

	private static short[] shortArray;

	public static void main(String args[]) {
		long[] longArray = getLongArray();
		float[] floatArray = getFloatArray();
		double[] doubleArray = getDoubleArray();
		byte[] byteArray = getByteArray();
		charArray = getCharArray();
		shortArray = getShortArray();
		for (int i = 0; i < 4; i++) {
			println(longArray[i]);
			println(floatArray[i]);
			println(doubleArray[i]);
			println(byteArray[i]);
			println(charArray[i]);
			println(shortArray[i]);
		}
	}

	public static long[] getLongArray() {
		long[] a = { 1234567890L, 12345678901L, 123456789012L, 1234567890123L };
		return a;
	}

	public static float[] getFloatArray() {
		float[] a = { 1.1f, 2.12f, 3.123f, 4.1234f };
		return a;
	}

	public static double[] getDoubleArray() {
		double[] a = { 1.12345, 12.123456, 123.123456, 1234.123456 };
		return a;
	}

	public static byte[] getByteArray() {
		byte[] a = { (byte) 1, (byte) 2, (byte) 3, (byte) 4 };
		return a;
	}

	public static char[] getCharArray() {
		char[] a = { 'a', 'b', 'c', 'd' };
		return a;
	}

	public static short[] getShortArray() {
		short[] a = { 256, 257, 258, 259 };
		return a;
	}
}
