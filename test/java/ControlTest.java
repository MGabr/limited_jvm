public class ControlTest {

	public void main(String args[]) {
		int a = 0;
		int b = 1;
		int c = 2;
		int d = -1;
		int e = 1;

		int f;

		if (a != 0) {
			f = 0;
		}
		if (b != 0) {
			f = 1;
		}

		if (b == 0) {
			f = 0;
		}
		if (a == 0) {
			f = 1;
		}

		if (d >= 0) {
			f = 0;
		}
		if (a >= 0) {
			f = 1;
		}

		if (a < 0) {
			f = 0;
		}
		if (b < 0) {
			f = 0;
		}
		if (d < 0) {
			f = 1;
		}

		if (b <= 0) {
			f = 0;
		}
		if (d <= 0) {
			f = 1;
		}
		
		if (a > 0) {
			f = 0;
		}
		if (d > 0) {
			f = 0;
		}
		if (b > 0) {
			f = 1;
		}

	}

}
