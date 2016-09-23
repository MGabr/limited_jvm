#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "core.h"
#include "testmode.h"
#include "warn.h"


#ifdef _TESTMODE_
	#define STATIC static
#else
	#define STATIC
#endif


void java(int argc, const char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: java <class file name>\n");
		exit(1);
	}

//	print_portability_warnings();

	struct ClassFile *cf = parse(argv[1]);

	run_main(cf);
}

STATIC int main(int argc, const char **argv) {
	java(argc, argv);

	return 0;
}

