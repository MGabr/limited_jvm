#include <gtest/gtest.h>

extern "C" {
	#include "../src/java.h"
}

TEST(ArrayTest, shouldPrint) {
	const char *args[2] = {"./java", "classes/ArrayTest"};
	testing::internal::CaptureStdout();
	java(2, args);
	EXPECT_STREQ(
		"1\n2\n3\n",
		testing::internal::GetCapturedStdout().c_str());
};

