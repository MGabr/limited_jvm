#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
}

class ReturnTest : public ::testing::Test {
	protected:
	virtual void SetUp() {
		cf = parse("classes/ReturnTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}

	virtual void TearDown() {
		free_vm(cf);
	}
};


TEST_F(ReturnTest, ireturn) {
	instrs_until_terminate = 3;
	run(ReturnTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};

TEST_F(ReturnTest, lreturn) {
	instrs_until_terminate = 7;
	run(ReturnTest::cf, getMethod());

	EXPECT_EQ(4294967296L, *((i8 *) (state.optop - 1)));
};

TEST_F(ReturnTest, freturn) {
	instrs_until_terminate = 11;
	run(ReturnTest::cf, getMethod());

	EXPECT_EQ(1.1f, *((float *) state.optop));
};

TEST_F(ReturnTest, dreturn) {
	instrs_until_terminate = 15;
	run(ReturnTest::cf, getMethod());

	EXPECT_EQ(1.11111111, *((double *) (state.optop - 1)));
};

