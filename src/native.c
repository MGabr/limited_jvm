#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "native.h"
#include "string_pool.h"
#include "log.h"

// -------------- native method implementations -------------

static void println_string(const char *s)
{
	DEBUG("Entered %s\n", __func__);
	printf("%s\n", s);
};

static void println_char(const char c)
{
	DEBUG("Entered %s\n", __func__);
	printf("%c\n", c);
}

static void println_byte(const i1 b)
{
	DEBUG("Entered %s\n", __func__);
	printf("%" PRId8 "\n", b);
}

static void println_short(const i2 s)
{
	DEBUG("Entered %s\n", __func__);
	printf("%" PRId16 "\n", s);
}

static void println_int(const i4 n)
{
	DEBUG("Entered %s\n", __func__);
	printf("%" PRId32"\n", n);
};

static void println_long(const i8 n)
{
	DEBUG("Entered %s\n", __func__);
	printf("%" PRId64 "\n", n);
};

// only 6 decimal digits
static void println_float(const float x)
{
	DEBUG("Entered %s\n", __func__);
	printf("%f\n", x);
}

static void println_double(const double x)
{
	DEBUG("Entered %s\n", __func__);
	printf("%f\n", x);
}

static void println_boolean(u4 b)
{
	if (b) {
		printf("true\n");
	} else {
		printf("false\n");
	}
}

// -------------- external native API -----------------------

const char *println_str;

const char *string_void_type_str;
const char *char_void_type_str;
const char *byte_void_type_str;
const char *short_void_type_str;
const char *int_void_type_str;
const char *long_void_type_str;
const char *float_void_type_str;
const char *double_void_type_str;
const char *boolean_void_type_str;

void init_natives(void) {
	DEBUG("Entered %s\n", __func__);

	println_str = add_string("println");

	string_void_type_str = add_string("(Ljava/lang/String;)V");
	char_void_type_str = add_string("(C)V");
	byte_void_type_str = add_string("(B)V");
	short_void_type_str = add_string("(S)V");
	int_void_type_str = add_string("(I)V");
	long_void_type_str = add_string("(J)V");
	float_void_type_str = add_string("(F)V");
	double_void_type_str = add_string("(D)V");
	boolean_void_type_str = add_string("(Z)V");
};

static void not_supported(struct r_method_info *method)
{
	ERROR("Native method %s%s not supported. (No JNI support)\n", 
			method->name, method->signature);
	exit(1);
}

void call_native(struct r_method_info *method, u4 *optop) {
	DEBUG("Entered %s(method->name=%s, method->signature=%s)\n",
		__func__, method->name, method->signature);

	if (method->name == println_str) {
		if (method->signature == string_void_type_str) {
			println_string(*(const char **) optop);
		} else if (method->signature == char_void_type_str) {
			println_char(*(const char *) optop);
		} else if (method->signature == byte_void_type_str) {
			println_byte(*(const i1 *) optop);
		} else if (method->signature == short_void_type_str) {
			println_short(*(const i2 *) optop);
		} else if (method->signature == int_void_type_str) {
			println_int(*(i4 *) optop);
		} else if (method->signature == long_void_type_str) {
			println_long(*(i8 *) (optop - 1));
		} else if (method->signature == float_void_type_str) {
			println_float(*(float *) optop);
		} else if (method->signature == double_void_type_str) {
			println_double(*(double *) (optop - 1));
		} else if (method->signature == boolean_void_type_str) {
			println_boolean(*optop);
		} else {
			not_supported(method);
		}
	} else {
		not_supported(method);
	}
};

