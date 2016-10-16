#include <gtest/gtest.h>

extern "C" {
	#include "../src/java.h"
}

TEST(PrimitivesArrayTest, shouldPrint) {
	const char *args[2] = {"./java", "classes/PrimitivesArrayTest"};
	testing::internal::CaptureStdout();
	java(2, args);
	EXPECT_STREQ(
		"1234567890\n1.100000\n1.123450\n1\na\n256\n12345678901\n2.120000\n12.123456\n2\nb\n257\n123456789012\n3.123000\n123.123456\n3\nc\n258\n1234567890123\n4.123400\n1234.123456\n4\nd\n259\n",
		testing::internal::GetCapturedStdout().c_str());
};

