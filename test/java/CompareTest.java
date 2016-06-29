public class CompareTest {

	public static void main(String args[]) {
		long l = 1L << 32;
		long l2 = 2L << 32;
		float f = 1.1f;
		float f2 = 2.2f;
		double d = 1.111111111;
		double d2 = 2.222222222;

		int _i;
		float fNaN = 0.0f / 0.0f;
		double dNaN = 0.0d / 0.0;

		if (l == l2) {
			_i = 0;
		}
		if (l2 == l) {
			_i = 0;
		}
		if (l != l) {
			_i = 0;
		}
		
		// fcmpl
		if (f == f2) {
			_i = 0;
		}
		if (f2 == f) {
			_i = 0;
		}
		if (f != f) {
			_i = 0;
		}
		if (f == fNaN) {
			_i = 0;
		}
		if (fNaN == f) {
			_i = 0;
		}

		// fcmpg
		if (f < f2) {
			_i = 1;
		}
		if (f2 < f) {
			_i = 0;
		}
		if (f < f) {
			_i = 0;
		}
		if (f < fNaN) {
			_i = 0;
		}
		if (fNaN < f) {
			_i = 0;
		}

		// dcmpl
		if (d == d2) {
			_i = 0;
		}
		if (d2 == d) {
			_i = 0;
		}
		if (d != d) {
			_i = 0;
		}
		if (d == dNaN) {
			_i = 0;
		}
		if (dNaN == d) {
			_i = 0;
		}

		// dcmpg
		if (d < d2) {
			_i = 1;
		}
		if (d2 < d) {
			_i = 0;
		}
		if (d < d) {
			_i = 0;
		}
		if (d < dNaN) {
			_i = 0;
		}
		if (dNaN < d) {
			_i = 0;
		}
	}

}
