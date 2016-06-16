public class IntegerLogicalTest {

	public static void main(String args[]) {
		int a = 3;
		int b = -3;
		int c = 2147483647; // Integer.MAX_VALUE
		int d = -2147483648; // Integer.MIN_VALUE
		int e;

		e = a << 2;
		e = a << 32;
		e = b << 2;
		e = c << 1;
		e = d << 1;

		e = a >> 1;
		e = a >> 32;
		e = b >> 2;
		e = c >> 1;
		e = d >> 1;

		e = a >>> 1;
		e = a >>> 32;
		e = b >>> 2;
		e = b >>> 1;
		e = d >>> 1;

		e = a & b;
		e = c & d;

		e = a | b;
		e = c | d;

		e = a ^ b;
		e = c ^ d;
	}

}
