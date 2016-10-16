#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
	#include "../src/opcodes.h"
}


class LongTest : public ::testing::Test {

	protected:
	virtual void SetUp() {
		cf = parse("classes/LongTest");
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


TEST_F(LongTest, lconst0) {
	instrs_until_terminate = 7;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(0L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, lconst1) {
	instrs_until_terminate = 9;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(1L, *((i8 *) (state.optop - 1)));
};


TEST_F(LongTest, lload) {
	// TODO
};

TEST_F(LongTest, lload0) {
	// TODO: ?
};

TEST_F(LongTest, lload1) {
	instrs_until_terminate = 11;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(4294967296LL, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, lload2) {
	// TODO: ?
};

TEST_F(LongTest, lload3) {
	instrs_until_terminate = 12;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(72L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, lstoreWithVal5) {
	instrs_until_terminate = 6;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(10L, *((i8 *) (state.frame + 5)));
};

TEST_F(LongTest, lstore0) {
	// TODO: ?
};

TEST_F(LongTest, lstore1) {
	instrs_until_terminate = 2;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(4294967296LL, *((i8 *) (state.frame + 1)));
};

TEST_F(LongTest, lstore2) {
	// TODO: ?
};

TEST_F(LongTest, lstore3) {
	instrs_until_terminate = 4;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(72L, *((i8 *) (state.frame + 3)));
};


TEST_F(LongTest, ladd) {
	instrs_until_terminate = 13;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(4294967368LL, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, lmul) {
	instrs_until_terminate = 17;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(309237645312LL, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, lsubAB) {
	instrs_until_terminate = 21;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(4294967224LL, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, lsubBA) {
	instrs_until_terminate = 25;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(-4294967224LL, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, ldivAB) {
	instrs_until_terminate = 29;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(59652323LL, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, ldivBA) {
	instrs_until_terminate = 33;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(0L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, lremBA) {
	instrs_until_terminate = 37;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(72L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, lremAB) {
	instrs_until_terminate = 41;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(40L, *((i8 *) (state.optop - 1)));
};

TEST_F(LongTest, lneg) {
	instrs_until_terminate = 44;
	run(LongTest::cf, getMethod());

	EXPECT_EQ(-4294967296LL, *((i8 *) (state.optop - 1)));
};

