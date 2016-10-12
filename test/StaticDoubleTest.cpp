#include <gtest/gtest.h>

extern "C" {
	#include "../src/java.h"
}

TEST(StaticDoubleTest, canAccessAndChangeStaticFields) {
	const char *args[2] = {"./java", "classes/StaticDoubleTest"};
	testing::internal::CaptureStdout();
	java(2, args);
	EXPECT_STREQ(
		"10.123400\n20.123400\n30.123400\n31.123400\n31.123400\n", 
		testing::internal::GetCapturedStdout().c_str());
};

