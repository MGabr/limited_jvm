#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
}


class MainTest : public ::testing::Test {
	protected:
	static void SetUpTestCase() {
		cf = parse("classes/HelloWorld");
	}

	static struct ClassFile *cf;
}; 

struct ClassFile *MainTest::cf = NULL;

TEST_F(MainTest, getMainMethod) {
	struct r_method_info *m0 = &MainTest::cf->methods[2];
	EXPECT_EQ(m0, get_main_method(MainTest::cf));
}

