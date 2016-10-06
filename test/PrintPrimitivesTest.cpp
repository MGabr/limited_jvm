#include <gtest/gtest.h>

extern "C" {
	#include "../src/java.h"
}

TEST(PrintPrimitivesTest, shouldPrintAllPrimitivesRight) {
	const char *args[2] = {"./java", "classes/PrintPrimitivesTest"};
	testing::internal::CaptureStdout();
	java(2, args);
	EXPECT_STREQ(
		"String\nc\n127\n32767\n2147483647\n2147483648\n10.123456\n100.123456\ntrue\nfalse\n",
		testing::internal::GetCapturedStdout().c_str());
};
