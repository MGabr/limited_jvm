public class LongTest {

	public static void main(String[] args) {
		long a = 1L << 32;
		long b = 72;
		long c = 10;
		long d = 0;
		long e = 1;

        c = a + b;

        c = a * b;

        c = a - b;
        c = b - a;

        c = a / b;
        c = b / a;

        c = b % a;
        c = a % b;

        c = -a;
	}

}
