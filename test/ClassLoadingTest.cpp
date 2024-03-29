#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
}

class ClassLoadingTest : public ::testing::Test {
	protected:
	virtual void SetUp() {
		cf = parse("classes/ClassLoadingTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}

	virtual void TearDown() {
		free_vm(cf);
	}
};

TEST_F(ClassLoadingTest, callStaticMethod1) {
	instrs_until_terminate = 4;

	testing::internal::CaptureStdout();
	run(ClassLoadingTest::cf, getMethod());

	EXPECT_STREQ("Called local static method\n", 
		testing::internal::GetCapturedStdout().c_str());
};

TEST_F(ClassLoadingTest, callStaticMethod2) {
	instrs_until_terminate = 8;

	testing::internal::CaptureStdout();
	run(ClassLoadingTest::cf, getMethod());

	EXPECT_STREQ(
		"Called local static method\nCalled static method in ClassLoadingTest2\n", 
		testing::internal::GetCapturedStdout().c_str());
};

TEST_F(ClassLoadingTest, callStaticMethod3) {
	instrs_until_terminate = 12;

	testing::internal::CaptureStdout();
	run(ClassLoadingTest::cf, getMethod());

	EXPECT_STREQ(
		"Called local static method\nCalled static method in ClassLoadingTest2\nCalled static method in ClassLoadingTest3\n", 
		testing::internal::GetCapturedStdout().c_str());
};

