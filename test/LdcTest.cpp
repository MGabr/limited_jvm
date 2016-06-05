#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}

class LdcTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/HelloWorld.class");
	}

	struct ClassFile *cf;
};

static u1 code_ldc1[4] = {LDC1, 2, LDC1, 2};

TEST_F(LdcTest, ldc1ShouldPush) {
	instrs_until_terminate = 1;
	run(LdcTest::cf, code_ldc1);
	EXPECT_EQ(start_state.optop + 1, state.optop);
}

TEST_F(LdcTest, ldc1ShouldResolveAndPush) {
	instrs_until_terminate = 1;
	run(LdcTest::cf, code_ldc1);
	EXPECT_STREQ("Hello, World", (const char *) *state.optop);
}

TEST_F(LdcTest, ldc1TwiceShouldPushResolved) {
	instrs_until_terminate = 2;
	run(LdcTest::cf, code_ldc1);
	EXPECT_STREQ("Hello, World", 
		(const char *) *(state.optop - 1));
	EXPECT_STREQ("Hello, World", (const char *) *state.optop);
}

// TODO: currently no ldc2 tests which test a high enough index

static u1 code_ldc2[6] = {LDC2, 0, 2, LDC2, 0, 2};

TEST_F(LdcTest, ldc2ShouldPush) {
	instrs_until_terminate = 1;
	run(LdcTest::cf, code_ldc2);
	EXPECT_EQ(start_state.optop + 1, state.optop);
}

TEST_F(LdcTest, ldc2ShouldResolveAndPush) {
	instrs_until_terminate = 1;
	run(LdcTest::cf, code_ldc2);
	EXPECT_STREQ("Hello, World", (const char *) *state.optop);
}

TEST_F(LdcTest, ldc2TwiceShouldPushResolved) {
	instrs_until_terminate = 2;
	run(LdcTest::cf, code_ldc2);
	EXPECT_STREQ("Hello, World",
		(const char *) *(state.optop - 1));
	EXPECT_STREQ("Hello, World", (const char *) *state.optop);
}



