#include <gtest/gtest.h>

extern "C" {
	#include "../src/java.h"
}

TEST(StaticTest, canAccessAndChangeStaticFields) {
	const char *args[2] = {"./java", "classes/StaticTest"};
	testing::internal::CaptureStdout();
	java(2, args);
	EXPECT_STREQ(
		"10\n20\n30\n31\n31\n", 
		testing::internal::GetCapturedStdout().c_str());
};

