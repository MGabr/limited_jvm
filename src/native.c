#include <stdio.h>
#include <stdlib.h>

#include "native.h"
#include "string_pool.h"
#include "log.h"

// -------------- native method implementations -------------

static void println(const char *s) {
	printf("%s\n", s);
};


// -------------- external native API -----------------------

const char *println_str;


void init_natives(void) {
	DEBUG("Entered %s\n", __func__);

	// currently only methods with different names,
	// so we don't also need to consider the method parameter types
	println_str = add_string("println");
};

void call_native(struct r_method_info *method, u4 *optop) {
	DEBUG("Entered %s(method->name=%s)\n", __func__, method->name);

	if (method->name == println_str) {
		println(*(const char **) optop);
	} else {
		ERROR("Native method %s%s not supported. (No JNI support)\n", 
			method->name, method->signature);
		exit(1);
	}
};



