public class SwitchPrintTest {

	private static native void println(String s);

	public static void main(String args[]) {
		int a = 2;
		int b = 10;

		switch (a) {
			case 1:
				println("1 wrong");
				break;
			case 2:
				println("2 right");
				break;
			case 3:
				println("3 wrong");
				break;
			case 40:
				println("40 wrong");
				break;
			case 50:
				println("50 wrong");
				break;
			case 6:
				println("6 wrong");
				break;
			case 70:
				println("70 wrong");
				break;
			case 8:
				println("8 wrong");
				break;
			case 90:
				println("90 wrong");
				break;
			default:
				println("default wrong");
		}

		switch (a) {
			case 1:
				println("wrong");
		}

		switch (a) {
			case 1:
				println("1 wrong");
				break;
			case 2:
				println("2 right");
				break;
			case 3:
				println("3 wrong");
				break;
		}

		switch (b) {
			case 1:
				println("b 1 wrong");
				break;
			case 2:
				println("b 2 wrong");
				break;
			case 3:
				println("b 3 wrong");
				break;
		}
	}
}
