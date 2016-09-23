#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


class ControlTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/ControlTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}

	virtual void TearDown() {
		free_vm(cf);
	}
};


TEST_F(ControlTest, ifeqTrue) {
	instrs_until_terminate = 12;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 20, state.pc);
};

TEST_F(ControlTest, ifeqFalse) {
	instrs_until_terminate = 14;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 24, state.pc);
};


TEST_F(ControlTest, ifneTrue) {
	instrs_until_terminate = 18;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 34, state.pc);
};

TEST_F(ControlTest, ifneFalse) {
	instrs_until_terminate = 20;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 38, state.pc);
};


TEST_F(ControlTest, ifltTrue) {
	instrs_until_terminate = 24;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 49, state.pc);
};

TEST_F(ControlTest, ifltFalse) {
	instrs_until_terminate = 26;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 53, state.pc);
};


TEST_F(ControlTest, ifgeEqual) {
	instrs_until_terminate = 30;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 63, state.pc);
};

TEST_F(ControlTest, ifgeGreater) {
	instrs_until_terminate = 32;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 70, state.pc);
};

TEST_F(ControlTest, ifgeFalse) {
	instrs_until_terminate = 34;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 75, state.pc);
};

TEST_F(ControlTest, ifgtTrue) {
	instrs_until_terminate = 38;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 85, state.pc);
};

TEST_F(ControlTest, ifgtFalse) {
	instrs_until_terminate = 40;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 90, state.pc);
};

TEST_F(ControlTest, ifleEqual) {
	instrs_until_terminate = 44;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 100, state.pc);
};

TEST_F(ControlTest, ifleLess) {
	instrs_until_terminate = 46;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 108, state.pc);
};

TEST_F(ControlTest, ifleFalse) {
	instrs_until_terminate = 48;
	run(ControlTest::cf, getMethod());

	EXPECT_EQ(start_state.pc + 112, state.pc);
};



