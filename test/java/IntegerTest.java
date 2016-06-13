public class IntegerTest {

	public static void main(String[] args) {
		int a = 2;
		int b = 3;
		int c = 6;
		int d = 1 << 8;
		int e = 1 << 16;

		c = a + b;

		c = a * b;

		c = a - b;
		c = b - a;

		c = a / b;
		c = b / a;

		c = b % a;
		c = a % b;

		c = -a;

		d = ++a;
		d = a++;
		d = a;

		e = --b;
		e = b--;
		e = b;
	}

}
