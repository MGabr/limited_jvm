#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


class LongLogicalTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/LongLogicalTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}

	virtual void TearDown() {
		free_vm(cf);
	}
};


TEST_F(LongLogicalTest, lshlPositive) {
	instrs_until_terminate = 11;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(12L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lshlIgnoredBits) {
	instrs_until_terminate = 15;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(3L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lshlNegative) {
	instrs_until_terminate = 19;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(-12L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lshlMaxValue) {
	instrs_until_terminate = 23;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(-2L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lshlMinValue) {
	instrs_until_terminate = 27;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(0L, *((i8 *) (state.optop - 1)));
};


TEST_F(LongLogicalTest, lshrPositive) {
	instrs_until_terminate = 31;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(1L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lshrIgnoredBits) {
	instrs_until_terminate = 35;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(3L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lshrNegative) {
	instrs_until_terminate = 39;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(-1L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lshrMaxValue) {
	instrs_until_terminate = 43;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(4611686018427387903L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lshrMinValue) {
	instrs_until_terminate = 47;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(-4611686018427387904L, *((i8 *) (state.optop - 1)));
};


TEST_F(LongLogicalTest, lushrPositive) {
	instrs_until_terminate = 51;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(1L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lushrIgnoredBits) {
	instrs_until_terminate = 55;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(3L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lushrNegative) {
	instrs_until_terminate = 59;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(4611686018427387903L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lushrMaxValue) {
	instrs_until_terminate = 63;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(9223372036854775806L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lushrMinValue) {
	instrs_until_terminate = 67;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(4611686018427387904L, *((i8 *) (state.optop - 1)));
};


TEST_F(LongLogicalTest, land) {
	instrs_until_terminate = 71;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(1L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, landMaxMinValues) {
	instrs_until_terminate = 75;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(0L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lor) {
	instrs_until_terminate = 79;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(-1L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lorMinMaxValues) {
	instrs_until_terminate = 83;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(-1L, *((i8 *) (state.optop - 1)));
};


TEST_F(LongLogicalTest, lxor) {
	instrs_until_terminate = 87;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(-2L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongLogicalTest, lxorMinMaxValues) {
	instrs_until_terminate = 91;
	run(LongLogicalTest::cf, getMethod());

	EXPECT_EQ(-1L, *((i8 *) (state.optop - 1)));
};


