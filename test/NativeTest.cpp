#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/native.h"
}

class NativeTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/HelloWorld");
		println_m = &cf->methods[1];

		init_natives();
	}

	struct ClassFile *cf;

	static struct r_method_info *println_m;

	virtual void TearDown() {
		free_class(cf);
	}
};

struct r_method_info *NativeTest::println_m = NULL;


TEST_F(NativeTest, println) {
	const char *s = "StringToPrint";

	testing::internal::CaptureStdout();
	call_native(println_m, (u4 *) &s);
	EXPECT_EQ("StringToPrint\n", testing::internal::GetCapturedStdout());
};


