public class IntegerNegativeTest {

	public static void main(String[] args) {
		int a = -2;
		int b = -3;
		int c;
		int d = 4;
		int e = -4;

		c = a + b;
		c = a + d;

		c = a - b;
		c = a - d;
		c = d - a;

		c = a * b;
		c = a * d;

		c = a / b;
		c = d / b;
		c = e / d;

		c = a % b;
		c = d % b;
		c = b % d;

		c = -a;

		c = ++a;
	
		c = --b;
	}
}
