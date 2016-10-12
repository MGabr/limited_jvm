import apackage.StaticTest3;

public class StaticTest {

	private static int afield = 10;

	private static native void println(int a);

	public static void main(String args[]) {
		println(afield);
		println(StaticTest2.anotherField);
		println(StaticTest3.yetAnotherField);
		StaticTest3.yetAnotherField++;
		println(StaticTest3.yetAnotherField);
		afield = StaticTest3.yetAnotherField;
		println(afield);
	}
}
