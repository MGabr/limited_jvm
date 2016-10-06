#include <gtest/gtest.h>

extern "C" {
	#include "../src/java.h"
}

TEST(HelloWorldTest, shouldPrint) {
	const char *args[2] = {"./java", "classes/HelloWorld"};
	testing::internal::CaptureStdout();
	java(2, args);
	EXPECT_STREQ(
		"Hello, World\n",
		testing::internal::GetCapturedStdout().c_str());
};

