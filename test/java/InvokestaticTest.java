public class InvokestaticTest {

	private static void invoke() {
		invoke("arg");
		return;
	}

	private static void invoke(String arg) {
		return;
	}

	public static void main(String[] args) {
		invoke();
		invoke("arg");
	}
}

