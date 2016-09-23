#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


class CastTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/CastTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}

	virtual void TearDown() {
		free_vm(cf);
	}
};


TEST_F(CastTest, i2l) {
	instrs_until_terminate = 10;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(1111111111L, *((i8 *) (state.optop - 1)));
};

TEST_F(CastTest, i2f) {
	instrs_until_terminate = 13;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(1.11111117E9f, *((float *) state.optop));
};

TEST_F(CastTest, i2d) {
	instrs_until_terminate = 16;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(1.111111111E9, *((double *) (state.optop - 1)));
};


TEST_F(CastTest, l2i) {
	instrs_until_terminate = 19;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(-2074054201, *((i4 *) state.optop));
};

TEST_F(CastTest, l2f) {
	instrs_until_terminate = 22;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(1.11111113E17f, *((float *) state.optop));
};

TEST_F(CastTest, l2d) {
	instrs_until_terminate = 25;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(1.11111111111111104E17, *((double *) (state.optop - 1)));
};


TEST_F(CastTest, f2i) {
	instrs_until_terminate = 28;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(1111111168, *((i4 *) state.optop));
};

TEST_F(CastTest, f2l) {
	instrs_until_terminate = 31;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(1111111168, *((i8 *) (state.optop - 1)));
};

TEST_F(CastTest, f2d) {
	instrs_until_terminate = 34;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(1.111111168E9, *((double *) (state.optop - 1)));
};


TEST_F(CastTest, d2i) {
	instrs_until_terminate = 37;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(2147483647, *((i4 *) state.optop));
};

TEST_F(CastTest, d2l) {
	instrs_until_terminate = 40;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(111111111111111104L, *((i8 *) (state.optop - 1)));
};

TEST_F(CastTest, d2f) {
	instrs_until_terminate = 43;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(1.11111113E17f, *((float *) state.optop));
};


TEST_F(CastTest, i2b) {
	instrs_until_terminate = 46;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(-57, *((i4 *) state.optop));
};

TEST_F(CastTest, i2c) {
	instrs_until_terminate = 49;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(13767, *((i4 *) state.optop));
};

TEST_F(CastTest, i2s) {
	instrs_until_terminate = 52;
	run(CastTest::cf, getMethod());

	EXPECT_EQ(13767, *((i4 *) state.optop));
};


