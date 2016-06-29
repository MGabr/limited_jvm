public class CastTest {

	public static void main(String args[]) {
		// 9 max significant decimal digits precision for float
		int i = 1111111111;
		int _i;

		// 17 max significant decimal digits precision for double
		long l = 111111111111111111L;
		long _l;

		// 9 max significant decimal digits precision for float
		float f = 1111111111.0f;
		float _f;

		// 17 max significant decimal digits precision for double
		double d = 111111111111111111.0d;
		double _d;

		byte _b;
		char _c;
		short _s;

		_l = (long) i;
		_f = (float) i;
		_d = (double) i;

		_i = (int) l;
		_f = (float) l;
		_d = (double) l;

		_i = (int) f;
		_l = (long) f;
		_d = (double) f;

		_i = (int) d;
		_l = (long) d;
		_f = (float) d;

		_b = (byte) i;
		_c = (char) i;
		_s = (short) i;
	}

}
