import apackage.StaticDoubleTest3;

public class StaticDoubleTest {

	private static double afield = 10.1234;

	private static native void println(double a);

	public static void main(String args[]) {
		println(afield);
		println(StaticDoubleTest2.anotherField);
		println(StaticDoubleTest3.yetAnotherField);
		StaticDoubleTest3.yetAnotherField++;
		println(StaticDoubleTest3.yetAnotherField);
		afield = StaticDoubleTest3.yetAnotherField;
		println(afield);
	}
}
