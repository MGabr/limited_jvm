#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


class FloatTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/FloatTest");
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

	virtual void TearDown() {
		free_vm(cf);
	}
};


TEST_F(FloatTest, fconst0) {
	u1 code[1] = {FCONST_0};
	instrs_until_terminate = 1;
	run(FloatTest::cf, getMethodWithCode(code));

	EXPECT_EQ(0.0f, *((float *) state.optop));
};

TEST_F(FloatTest, fconst1) {
	u1 code[1] = {FCONST_1};
	instrs_until_terminate = 1;
	run(FloatTest::cf, getMethodWithCode(code));

	EXPECT_EQ(1.0f, *((float *) state.optop));
};

TEST_F(FloatTest, fconst2) {
	instrs_until_terminate = 1;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(2.0f, *((float *) state.optop));
};


TEST_F(FloatTest, fload) {
	// TODO
};

TEST_F(FloatTest, fload0) {
	// TODO: ?
};

// requires fstore to work
TEST_F(FloatTest, fload1) {
	instrs_until_terminate = 11;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(2.0f, *((float *) state.optop));
};

// requires fstore to work
TEST_F(FloatTest, fload2) {
	instrs_until_terminate = 12;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(3.3f, *((float *) state.optop));
};

TEST_F(FloatTest, fload3) {
	// TODO: ?
};

// requires ldc to work
TEST_F(FloatTest, fstoreWithVal4) {
	instrs_until_terminate = 8;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(257.0f, *((float *) state.frame + 4));
};

TEST_F(FloatTest, fstore0) {
	// TODO: ?
};

// requires fconst to work
TEST_F(FloatTest, fstore1) {
	instrs_until_terminate = 2;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(2.0f, *((float *) state.frame + 1));
};

// requires fconst to work
TEST_F(FloatTest, fstore2) {
	instrs_until_terminate = 4;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(3.3f, *((float *) state.frame + 2));
};

// requires fconst to work
TEST_F(FloatTest, fstore3) {
	instrs_until_terminate = 6;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(6.6f, *((float *) state.frame + 3));
};


TEST_F(FloatTest, fadd) {
	instrs_until_terminate = 13;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(5.3f, *((float *) state.optop));
};

TEST_F(FloatTest, fmul) {
	instrs_until_terminate = 17;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(6.6f, *((float *) state.optop));
};

TEST_F(FloatTest, fsubAB) {
	instrs_until_terminate = 21;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(-1.3f, *((float *) state.optop));
};

TEST_F(FloatTest, fsubBA) {
	instrs_until_terminate = 25;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(1.3f, *((float *) state.optop));
};

TEST_F(FloatTest, fdivAB) {
	instrs_until_terminate = 29;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(0.6060606f, *((float *) state.optop));
};

TEST_F(FloatTest, fdivBA) {
	instrs_until_terminate = 33;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(1.65f, *((float *) state.optop));
};

TEST_F(FloatTest, fremBA) {
	instrs_until_terminate = 37;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(1.3f, *((float *) state.optop));
};

TEST_F(FloatTest, fremAB) {
	instrs_until_terminate = 41;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(2.0f, *((float *) state.optop));
};

TEST_F(FloatTest, fneg) {
	instrs_until_terminate = 44;
	run(FloatTest::cf, getMethod());

	EXPECT_EQ(-2.0f, *((float *) state.optop));
};

