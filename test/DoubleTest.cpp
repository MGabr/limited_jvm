#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


class DoubleTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/DoubleTest");
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


TEST_F(DoubleTest, dconst0) {
	instrs_until_terminate = 7;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(0.0, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, dconst1) {
	instrs_until_terminate = 9;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(1.0, *((double *) (state.optop - 1)));
};


TEST_F(DoubleTest, dload) {
	// TODO
};

TEST_F(DoubleTest, dload0) {
	// TODO: ?
};

TEST_F(DoubleTest, dload1) {
	instrs_until_terminate = 11;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(2.0, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, dload2) {
	// TODO: ?
};

TEST_F(DoubleTest, dload3) {
	instrs_until_terminate = 12;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(3.333, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, dstoreWithVal5) {
	instrs_until_terminate = 6;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(6.666, *((double *) (state.frame + 5)));
};

TEST_F(DoubleTest, dstore0) {
	// TODO: ?
};

TEST_F(DoubleTest, dstore1) {
	instrs_until_terminate = 2;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(2.0, *((double *) (state.frame + 1)));
};

TEST_F(DoubleTest, dstore2) {
	// TODO: ?
};

TEST_F(DoubleTest, dstore3) {
	instrs_until_terminate = 4;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(3.333, *((double *) (state.frame + 3)));
};


TEST_F(DoubleTest, dadd) {
	instrs_until_terminate = 13;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(5.333, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, dmul) {
	instrs_until_terminate = 17;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(6.666, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, dsubAB) {
	instrs_until_terminate = 21;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(-1.3330000000000002, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, dsubBA) {
	instrs_until_terminate = 25;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(1.3330000000000002, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, ddivAB) {
	instrs_until_terminate = 29;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(0.6000600060006, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, ddivBA) {
	instrs_until_terminate = 33;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(1.6665, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, dremBA) {
	instrs_until_terminate = 37;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(1.3330000000000002, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, dremAB) {
	instrs_until_terminate = 41;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(2.0, *((double *) (state.optop - 1)));
};

TEST_F(DoubleTest, dneg) {
	instrs_until_terminate = 44;
	run(DoubleTest::cf, getMethod());

	EXPECT_EQ(-2.0, *((double *) (state.optop - 1)));
};

