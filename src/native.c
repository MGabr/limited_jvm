#include <stdio.h>
#include <stdlib.h>

#include "native.h"
#include "string_pool.h"

// -------------- native method implementations -------------

static void println(const char *s) {
	printf("%s\n", s);
};


// -------------- external native API -----------------------

const char *println_str;


void init_natives(void) {
	// currently only methods with different names,
	// so we don't also need to consider the method parameter types
	println_str = add_string("println");
};

void call_native(struct r_method_info *method, u4 *optop) {
	if (method->name == println_str) {
		println(*(const char **) optop);
	} else {
		fprintf(stderr, "Native method %s%s not supported. (No JNI support)\n", 
			method->name, method->signature);
		exit(1);
	}
};



