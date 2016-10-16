#include <gtest/gtest.h>

extern "C" {
	#include "../src/java.h"
}

TEST(SwitchPrintTest, shouldComputeRight) {
	const char *args[2] = {"./java", "classes/SwitchPrintTest"};
	testing::internal::CaptureStdout();
	java(2, args);
	EXPECT_STREQ(
		"2 right\n2 right\n",
		testing::internal::GetCapturedStdout().c_str());
};

