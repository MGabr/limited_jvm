public class ReturnTest {

	public static int ireturn() {
		return 1;
	}

	public static long lreturn() {
		return 1L << 32;
	}

	public static float freturn() {
		return 1.1f;
	}

	public static double dreturn() {
		return 1.11111111d;
	}

	public static void main(String args[]) {
		ireturn();
		lreturn();
		freturn();
		dreturn();
	}
}
