#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
}

class SwitchTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/SwitchTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}
};


TEST_F(SwitchTest, lookupswitch) {
	instrs_until_terminate = 6;
	run(SwitchTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 93, state.pc);
};

TEST_F(SwitchTest, lookupswitchDefault) {
	instrs_until_terminate = 10;
	run(SwitchTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 162, state.pc);
};

TEST_F(SwitchTest, tableswitch) {
	instrs_until_terminate = 12;
	run(SwitchTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 193, state.pc);
};

TEST_F(SwitchTest, tableswitchDefault) {
	instrs_until_terminate = 16;
	run(SwitchTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 240, state.pc);
};

