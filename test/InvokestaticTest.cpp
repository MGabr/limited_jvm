#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/core.h"
	#include "../src/testmode.h"
}

// TODO: common header?
#define GET_OLDLOCALC(frame, localc) (*((u4 *) frame + localc))
#define GET_OLDFRAME(frame, localc) (*((u4 **) frame + localc + 1))
#define GET_OLDPC(frame, localc) (*((u1 **) frame + localc + 2))
#define GET_OLDC(frame, localc) (*((struct ClassFile **) frame + localc + 3))


class InvokestaticTest : public ::testing::Test {
	protected:
	virtual void SetUp() {
		// for details of the class file to understand test constants
		// javap -v -verbose -c InvokestaticTest.class
		cf = parse("classes/InvokestaticTest");
	}

	struct ClassFile *cf;

	virtual void TearDown() {
		free_vm(cf);
	}
};

// many of the other tests are based on functionality tested here
TEST_F(InvokestaticTest, invokeNoArgRightFrameIncrease) {
	instrs_until_terminate = 1;
	run(cf, get_main_method(cf));

	EXPECT_EQ(start_state.optop + 1, state.frame);
};


TEST_F(InvokestaticTest, rightStartAndOldPc) {
	instrs_until_terminate = 1;
	struct r_method_info *main = get_main_method(cf);
	run(cf, main);

	EXPECT_EQ(main->c_attr->code, start_state.pc);
	EXPECT_EQ(main->c_attr->code + 3, GET_OLDPC(state.frame, state.localc));
};

TEST_F(InvokestaticTest, rightPcChange) {
	instrs_until_terminate = 1;
	struct r_method_info *main = get_main_method(cf);
	run(cf, main);
	struct r_method_info *called_m = &cf->methods[1];

	EXPECT_EQ(called_m->c_attr->code, state.pc);
};


TEST_F(InvokestaticTest, sameClassAndCp) {
	instrs_until_terminate = 1;
	run(cf, get_main_method(cf));

	EXPECT_EQ(start_state.c, state.c);
	EXPECT_EQ(start_state.cp, state.cp);
};


TEST_F(InvokestaticTest, rightOldFrame) {
	instrs_until_terminate = 1;
	run(cf, get_main_method(cf));

	EXPECT_EQ(start_state.frame, GET_OLDFRAME(state.frame, state.localc));
};


TEST_F(InvokestaticTest, rightStartAndOldLocalc) {
	instrs_until_terminate = 1;
	run(cf, get_main_method(cf));

	EXPECT_EQ(1, start_state.localc);
	EXPECT_EQ(1, GET_OLDLOCALC(state.frame, state.localc));
}

TEST_F(InvokestaticTest, invokeNoArgSetsRightLocalc) {
	instrs_until_terminate = 1;
	run(cf, get_main_method(cf));

	EXPECT_EQ(0, state.localc);
};

TEST_F(InvokestaticTest, invokeArgSetsRightLocalc) {
	instrs_until_terminate = 3; // step through invoke to invoke("arg")
	run(cf, get_main_method(cf));

	EXPECT_EQ(1, state.localc);
};


TEST_F(InvokestaticTest, invokeArgShouldPushArgument) {
	instrs_until_terminate = 3; // step through invoke to invoke("arg")
	run(cf, get_main_method(cf));

	EXPECT_STREQ("arg", *((const char **) state.frame));
};


// TODO: test with additional local variables

TEST_F(InvokestaticTest, returnRestoresFrame) {
	instrs_until_terminate = 5;
	run(cf, get_main_method(cf));

	EXPECT_EQ(start_state.frame, state.frame);
};

TEST_F(InvokestaticTest, returnRestoresPc) {
	instrs_until_terminate = 5;
	run(cf, get_main_method(cf));

	EXPECT_EQ(start_state.pc + 3, state.pc);
};

TEST_F(InvokestaticTest, returnSameClassAndCp) {
	instrs_until_terminate = 5;
	run(cf, get_main_method(cf));

	EXPECT_EQ(start_state.c, state.c);
	EXPECT_EQ(start_state.cp, state.cp);
};

TEST_F(InvokestaticTest, returnRestoresLocalc) {
	instrs_until_terminate = 5;
	run(cf, get_main_method(cf));

	EXPECT_EQ(start_state.localc, state.localc);	
};

TEST_F(InvokestaticTest, returnRestoresOptop) {
	instrs_until_terminate = 5;
	run(cf, get_main_method(cf));

	EXPECT_EQ(start_state.optop, state.optop);
};

