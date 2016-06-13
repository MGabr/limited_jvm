#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"

	#include <stdio.h>
}


class IntegerTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/IntegerTest");
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
};


TEST_F(IntegerTest, iconst0) {
	u1 code[1] = {ICONST_0};
	instrs_until_terminate = 1;
	run(IntegerTest::cf, getMethodWithCode(code));

	EXPECT_EQ(0, *state.optop);
};

TEST_F(IntegerTest, iconst1) {
	u1 code[1] = {ICONST_1};
	instrs_until_terminate = 1;
	run(IntegerTest::cf, getMethodWithCode(code));

	EXPECT_EQ(1, *state.optop);
};

TEST_F(IntegerTest, iconst2) {
	instrs_until_terminate = 1;
	run(IntegerTest::cf, getMethodWithCodeOffset(0));

	EXPECT_EQ(2, *state.optop);
};

TEST_F(IntegerTest, iconst3) {
	instrs_until_terminate = 1;
	run(IntegerTest::cf, getMethodWithCodeOffset(2));

	EXPECT_EQ(3, *state.optop);
};

TEST_F(IntegerTest, iconst4) {
	u1 code[1] = {ICONST_4};
	instrs_until_terminate = 1;
	run(IntegerTest::cf, getMethodWithCode(code));

	EXPECT_EQ(4, *state.optop);
};

TEST_F(IntegerTest, iconst5) {
	u1 code[1] = {ICONST_5};
	instrs_until_terminate = 1;
	run(IntegerTest::cf, getMethodWithCode(code));

	EXPECT_EQ(5, *state.optop);
};


TEST_F(IntegerTest, bipushWithVal6) {
	u1 code[1] = {BIPUSH};
	instrs_until_terminate = 1;
	run(IntegerTest::cf, getMethodWithCodeOffset(4));

	EXPECT_EQ(6, *state.optop);
};

TEST_F(IntegerTest, sipushWithVal257) {
	u1 code[1] = {SIPUSH};
	instrs_until_terminate = 1;
	run(IntegerTest::cf, getMethodWithCodeOffset(7));

	EXPECT_EQ(257, *state.optop);
};


