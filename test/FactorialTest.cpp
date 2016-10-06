#include <gtest/gtest.h>

extern "C" {
	#include "../src/java.h"
}

TEST(FactorialTest, shouldComputeRight) {
	const char *args[2] = {"./java", "classes/FactorialTest"};
	testing::internal::CaptureStdout();
	java(2, args);
	EXPECT_STREQ(
		"3628800\n",
		testing::internal::GetCapturedStdout().c_str());
};

