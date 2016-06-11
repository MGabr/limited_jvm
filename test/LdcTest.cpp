#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


static u1 code_ldc1[4] = {LDC1, 2, LDC1, 2};

static u1 code_ldc2[6] = {LDC2, 0, 2, LDC2, 0, 2};


class LdcTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/HelloWorld");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getLdc1Method() {
		struct r_method_info *m = get_main_method(cf);
		m->c_attr->code = code_ldc1;
		return m;
	}

	virtual struct r_method_info *getLdc2Method() {
		struct r_method_info *m = get_main_method(cf);
		m->c_attr->code = code_ldc2;
		return m;
	}
};

TEST_F(LdcTest, ldc1ShouldPush) {
	instrs_until_terminate = 1;
	run(LdcTest::cf, LdcTest::getLdc1Method());
	EXPECT_EQ(start_state.optop + 1, state.optop);
}

TEST_F(LdcTest, ldc1ShouldResolveAndPush) {
	instrs_until_terminate = 1;
	run(LdcTest::cf, LdcTest::getLdc1Method());
	EXPECT_STREQ("Hello, World", (const char *) *state.optop);
}

TEST_F(LdcTest, ldc1TwiceShouldPushResolved) {
	instrs_until_terminate = 2;
	run(LdcTest::cf, LdcTest::getLdc1Method());
	EXPECT_STREQ("Hello, World", 
		(const char *) *(state.optop - 1));
	EXPECT_STREQ("Hello, World", (const char *) *state.optop);
}

// TODO: currently no ldc2 tests which test a high enough index

TEST_F(LdcTest, ldc2ShouldPush) {
	instrs_until_terminate = 1;
	run(LdcTest::cf, LdcTest::getLdc2Method());
	EXPECT_EQ(start_state.optop + 1, state.optop);
}

TEST_F(LdcTest, ldc2ShouldResolveAndPush) {
	instrs_until_terminate = 1;
	run(LdcTest::cf, LdcTest::getLdc2Method());
	EXPECT_STREQ("Hello, World", (const char *) *state.optop);
}

TEST_F(LdcTest, ldc2TwiceShouldPushResolved) {
	instrs_until_terminate = 2;
	run(LdcTest::cf, LdcTest::getLdc2Method());
	EXPECT_STREQ("Hello, World",
		(const char *) *(state.optop - 1));
	EXPECT_STREQ("Hello, World", (const char *) *state.optop);
}


