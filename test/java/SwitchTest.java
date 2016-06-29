public class SwitchTest {

	public static void main(String args[]) {
		int a = 2;
		int b = 10;

		int _i;

		switch (a) {
			case 1:
				_i = 1;
				break;
			case 2:
				_i = 2;
				break;
			case 3:
				_i = 3;
				break;
			case 40:
				_i = 4;
				break;
			case 50:
				_i = 5;
				break;
			case 6:
				_i = 6;
				break;
			case 70:
				_i = 7;
				break;
			case 8:
				_i = 8;
				break;
			case 90:
				_i = 9;
				break;
			default:
				_i = 0;
		}

		switch (a) {
			case 1:
				_i = 1;
		}

		switch (a) {
			case 1:
				_i = 1;
				break;
			case 2:
				_i = 2;
				break;
			case 3:
				_i = 3;
				break;
		}

		switch (b) {
			case 1:
				_i = 1;
				break;
			case 2:
				_i = 2;
				break;
			case 3:
				_i = 3;
				break;
		}
	}

}
