#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


class IntegerNegativeTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/IntegerNegativeTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethodWithCodeOffset(int offset) {
		struct r_method_info *m = get_main_method(cf);
		m->c_attr->code = m->c_attr->code + offset;
		return m;
	}

	virtual struct r_method_info *getMethodWithCode(u1 *code) {
		struct r_method_info *m = get_main_method(cf);
		m->c_attr->code = code;
		return m;
	}

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}
};


TEST_F(IntegerNegativeTest, bipush) {
	instrs_until_terminate = 1;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-2, *state.optop);
};

// TODO: Test sipush

TEST_F(IntegerNegativeTest, iadd) {
	instrs_until_terminate = 11;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-5, *state.optop);
};

TEST_F(IntegerNegativeTest, iaddOnePositive) {
	instrs_until_terminate = 15;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(2, *state.optop);
};


TEST_F(IntegerNegativeTest, isub) {
	instrs_until_terminate = 19;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(1, *state.optop);
};

TEST_F(IntegerNegativeTest, isubSubtrahendPositive) {
	instrs_until_terminate = 23;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-6, *state.optop);
};

TEST_F(IntegerNegativeTest, isubMinuendPositive) {
	instrs_until_terminate = 27;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(6, *state.optop);
};


TEST_F(IntegerNegativeTest, imul) {
	instrs_until_terminate = 31;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(6, *state.optop);
};

TEST_F(IntegerNegativeTest, imulOnePositive) {
	instrs_until_terminate = 35;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-8, *state.optop);
};


TEST_F(IntegerNegativeTest, idiv) {
	instrs_until_terminate = 39;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(0, *state.optop);
};

TEST_F(IntegerNegativeTest, idivDividendPositive) {
	instrs_until_terminate = 43;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-1, *state.optop);
};

TEST_F(IntegerNegativeTest, idivDivisorPositive) {
	instrs_until_terminate = 47;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-1, *state.optop);
};


TEST_F(IntegerNegativeTest, irem) {
	instrs_until_terminate = 51;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-2, *state.optop);
};

TEST_F(IntegerNegativeTest, iremDividendPositive) {
	instrs_until_terminate = 55;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(1, *state.optop);
};

TEST_F(IntegerNegativeTest, iremDivisorPositive) {
	instrs_until_terminate = 59;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-3, *state.optop);
};


TEST_F(IntegerNegativeTest, ineg) {
	instrs_until_terminate = 62;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(2, *state.optop);
};


TEST_F(IntegerNegativeTest, iinc) {
	instrs_until_terminate = 64;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-1, *(state.frame + 1));
};

TEST_F(IntegerNegativeTest, iincNegative) {
	instrs_until_terminate = 67;
	run(IntegerNegativeTest::cf, getMethod());

	EXPECT_EQ(-4, *(state.frame + 2));
};

