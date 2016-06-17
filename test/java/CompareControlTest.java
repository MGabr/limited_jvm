public class CompareControlTest {

	public void main(String args[]) {
		int a = 1;
		int b = 2;
		int c = 3;
		int d = -1;
		int e = 1;

		int f;

		if (a != 1) {
			f = 0;
		}
		if (b != 1) {
			f = 1;
		}

		if (b == 1) {
			f = 0;
		}
		if (a == 1) {
			f = 1;
		}

		if (d >= 1) {
			f = 0;
		}
		if (a >= 1) {
			f = 1;
		}

		if (a < 1) {
			f = 0;
		}
		if (b < 1) {
			f = 0;
		}
		if (d < 1) {
			f = 1;
		}

		if (b <= 1) {
			f = 0;
		}
		if (d <= 1) {
			f = 1;
		}
		
		if (a > 1) {
			f = 0;
		}
		if (d > 1) {
			f = 0;
		}
		if (b > 1) {
			f = 1;
		}

	}

}
