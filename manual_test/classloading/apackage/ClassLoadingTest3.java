package apackage;

public class ClassLoadingTest3 {

	private static native void println(String s);

	public static void staticMethod() {
		println("Called static method in ClassLoadingTest3");
	}
}
