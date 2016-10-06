import apackage.ClassLoadingTest3;

public class ClassLoadingTest {

	private static native void println(String s);

	public static void localStaticMethod() {
		println("Called local static method");
	}

	public static void main(String args[]) {
		localStaticMethod();
		ClassLoadingTest2.staticMethod();
		ClassLoadingTest3.staticMethod();
	}

}
