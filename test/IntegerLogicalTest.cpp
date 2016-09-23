#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


class IntegerLogicalTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/IntegerLogicalTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}

	virtual void TearDown() {
		free_vm(cf);
	}
};


TEST_F(IntegerLogicalTest, ishlPositive) {
	instrs_until_terminate = 11;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(12, *state.optop);
};

TEST_F(IntegerLogicalTest, ishlIgnoredBits) {
	instrs_until_terminate = 15;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(3, *state.optop);
};

TEST_F(IntegerLogicalTest, ishlNegative) {
	instrs_until_terminate = 19;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(-12, *state.optop);
};

TEST_F(IntegerLogicalTest, ishlMaxValue) {
	instrs_until_terminate = 23;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(-2, *state.optop);
};

TEST_F(IntegerLogicalTest, ishlMinValue) {
	instrs_until_terminate = 27;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(0, *state.optop);
};


TEST_F(IntegerLogicalTest, ishrPositive) {
	instrs_until_terminate = 31;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(1, *state.optop);
};

TEST_F(IntegerLogicalTest, ishrIgnoredBits) {
	instrs_until_terminate = 35;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(3, *state.optop);
};

TEST_F(IntegerLogicalTest, ishrNegative) {
	instrs_until_terminate = 39;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(-1, *state.optop);
};

TEST_F(IntegerLogicalTest, ishrMaxValue) {
	instrs_until_terminate = 43;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(1073741823, *state.optop);
};

TEST_F(IntegerLogicalTest, ishrMinValue) {
	instrs_until_terminate = 47;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(-1073741824, *state.optop);
};


TEST_F(IntegerLogicalTest, iushrPositive) {
	instrs_until_terminate = 51;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(1, *state.optop);
};

TEST_F(IntegerLogicalTest, iushrIgnoredBits) {
	instrs_until_terminate = 55;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(3, *state.optop);
};

TEST_F(IntegerLogicalTest, iushrNegative) {
	instrs_until_terminate = 59;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(1073741823, *state.optop);
};

TEST_F(IntegerLogicalTest, iushrMaxValue) {
	instrs_until_terminate = 63;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(2147483646, *state.optop);
};

TEST_F(IntegerLogicalTest, iushrMinValue) {
	instrs_until_terminate = 67;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(1073741824, *state.optop);
};


TEST_F(IntegerLogicalTest, iand) {
	instrs_until_terminate = 71;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(1, *state.optop);
};

TEST_F(IntegerLogicalTest, iandMaxMinValues) {
	instrs_until_terminate = 75;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(0, *state.optop);
};

TEST_F(IntegerLogicalTest, ior) {
	instrs_until_terminate = 79;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(-1, *state.optop);
};

TEST_F(IntegerLogicalTest, iorMinMaxValues) {
	instrs_until_terminate = 83;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(-1, *state.optop);
};


TEST_F(IntegerLogicalTest, ixor) {
	instrs_until_terminate = 87;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(-2, *state.optop);
};

TEST_F(IntegerLogicalTest, ixorMinMaxValues) {
	instrs_until_terminate = 91;
	run(IntegerLogicalTest::cf, getMethod());

	EXPECT_EQ(-1, *state.optop);
};


