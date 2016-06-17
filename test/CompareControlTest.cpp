#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


class CompareControlTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/CompareControlTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}
};


TEST_F(CompareControlTest, ificmpeqTrue) {
	instrs_until_terminate = 13;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 21, state.pc);
};

TEST_F(CompareControlTest, ificmpeqFalse) {
	instrs_until_terminate = 16;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 26, state.pc);
};


TEST_F(CompareControlTest, ificmpneTrue) {
	instrs_until_terminate = 21;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 37, state.pc);
};

TEST_F(CompareControlTest, ificmpneFalse) {
	instrs_until_terminate = 24;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 42, state.pc);
};


TEST_F(CompareControlTest, ificmpltTrue) {
	instrs_until_terminate = 29;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 54, state.pc);
};

TEST_F(CompareControlTest, ificmpltFalse) {
	instrs_until_terminate = 32;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 59, state.pc);
};


TEST_F(CompareControlTest, ificmpgeEqual) {
	instrs_until_terminate = 37;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 70, state.pc);
};

TEST_F(CompareControlTest, ificmpgeGreater) {
	instrs_until_terminate = 40;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 78, state.pc);
};

TEST_F(CompareControlTest, ificmpgeFalse) {
	instrs_until_terminate = 43;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 84, state.pc);
};

TEST_F(CompareControlTest, ificmpgtTrue) {
	instrs_until_terminate = 48;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 95, state.pc);
};

TEST_F(CompareControlTest, ificmpgtFalse) {
	instrs_until_terminate = 51;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 101, state.pc);
};


TEST_F(CompareControlTest, ificmpleEqual) {
	instrs_until_terminate = 56;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 112, state.pc);
};

TEST_F(CompareControlTest, ificmpleLess) {
	instrs_until_terminate = 59;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 121, state.pc);
};

TEST_F(CompareControlTest, ificmpleFalse) {
	instrs_until_terminate = 62;
	run(CompareControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 126, state.pc);
};



