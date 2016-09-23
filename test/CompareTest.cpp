#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
}

class CompareTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/CompareTest");
	}

	struct ClassFile *cf;

	virtual struct r_method_info *getMethod(void) {
		return get_main_method(cf);
	}

	virtual void TearDown() {
		free_vm(cf);
	}
};


TEST_F(CompareTest, lcmpSmaller) {
	instrs_until_terminate = 19;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(-1, *((i4 *) state.optop));
};

TEST_F(CompareTest, lcmpLarger) {
	instrs_until_terminate = 23;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};

TEST_F(CompareTest, lcmpEqual) {
	instrs_until_terminate = 27;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(0, *((i4 *) state.optop));
};


TEST_F(CompareTest, fcmplSmaller) {
	instrs_until_terminate = 31;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(-1, *((i4 *) state.optop));
};

TEST_F(CompareTest, fcmplLarger) {
	instrs_until_terminate = 35;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};

TEST_F(CompareTest, fcmplEqual) {
	instrs_until_terminate = 39;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(0, *((i4 *) state.optop));
};

TEST_F(CompareTest, fcmplNaNRight) {
	instrs_until_terminate = 43;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(-1, *((i4 *) state.optop));
};

TEST_F(CompareTest, fcmplNaNLeft) {
	instrs_until_terminate = 47;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(-1, *((i4 *) state.optop));
};


TEST_F(CompareTest, fcmpgSmaller) {
	instrs_until_terminate = 51;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(-1, *((i4 *) state.optop));
};

TEST_F(CompareTest, fcmpgLarger) {
	instrs_until_terminate = 57;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};

TEST_F(CompareTest, fcmpgEqual) {
	instrs_until_terminate = 61;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(0, *((i4 *) state.optop));
};

TEST_F(CompareTest, fcmpgNaNRight) {
	instrs_until_terminate = 65;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};

TEST_F(CompareTest, fcmpgNaNLeft) {
	instrs_until_terminate = 69;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};


TEST_F(CompareTest, dcmplSmaller) {
	instrs_until_terminate = 73;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(-1, *((i4 *) state.optop));
};

TEST_F(CompareTest, dcmplLarger) {
	instrs_until_terminate = 77;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};

TEST_F(CompareTest, dcmplEqual) {
	instrs_until_terminate = 81;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(0, *((i4 *) state.optop));
};

TEST_F(CompareTest, dcmplNaNRight) {
	instrs_until_terminate = 85;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(-1, *((i4 *) state.optop));
};

TEST_F(CompareTest, dcmplNaNLeft) {
	instrs_until_terminate = 89;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(-1, *((i4 *) state.optop));
};


TEST_F(CompareTest, dcmpgSmaller) {
	instrs_until_terminate = 93;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(-1, *((i4 *) state.optop));
};

TEST_F(CompareTest, dcmpgLarger) {
	instrs_until_terminate = 99;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};

TEST_F(CompareTest, dcmpgEqual) {
	instrs_until_terminate = 103;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(0, *((i4 *) state.optop));
};

TEST_F(CompareTest, dcmpgNaNRight) {
	instrs_until_terminate = 107;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};

TEST_F(CompareTest, dcmpgNaNLeft) {
	instrs_until_terminate = 111;
	run(CompareTest::cf, getMethod());

	EXPECT_EQ(1, *((i4 *) state.optop));
};

