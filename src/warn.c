#include <stdio.h>

#include "warn.h"

void print_portability_warnings(void) {
	if (sizeof(double) != 8) {
		fprintf(stderr, "WARNING: Different double size: %zu\n", 
			sizeof(double));
	}

#ifndef __STDC_IEC_599__
	if (sizeof(float) != 4) {
		fprintf(stderr, "WARNING: Different float size: %zu\n", sizeof(float));
	}
	if (sizeof(double) != 8) {
		fprintf(stderr, "WARNING: Different double size: %zu\n", 
			sizeof(double));
	}
	fprintf(stderr, "WARNING: Possibly not IEEE conforming floating point operations: __STDC_IEC_599__ not defined\n");
#endif

#ifndef NAN
	fprintf(stderr, "WARNING: Not IEEE conforming floating point operations: NAN not defined.\n");
#endif

#ifndef INFINITY
	fprintf(stderr, "WARNING: Not IEEE conforming floating point operations: INFINITY not defined.\n");
#endif
}

