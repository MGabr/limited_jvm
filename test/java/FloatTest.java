public class FloatTest {

	public static void main(String[] args) {
		float a = 2;
		float b = 3.3f;
		float c = 6.6f;
		float d = (1 << 8) + 1;
		float e = 1 << 16;

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

		e = --b;
	}

}
