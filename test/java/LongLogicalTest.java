public class LongLogicalTest {

	public static void main(String args[]) {
		long a = 3;
		long b = -3;
		long c = 9223372036854775807L; // Long.MAX_VALUE
		long d = -9223372036854775808L; // Long.MIN_VALUE
		long e;

		e = a << 2;
		e = a << 64;
		e = b << 2;
		e = c << 1;
		e = d << 1;

		e = a >> 1;
		e = a >> 64;
		e = b >> 2;
		e = c >> 1;
		e = d >> 1;

		e = a >>> 1;
		e = a >>> 64;
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
