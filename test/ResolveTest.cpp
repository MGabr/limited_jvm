#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/resolve.h"
	#include <stddef.h>
}

class ResolveTest : public ::testing::Test {
	protected:
	virtual void SetUp() {
		cf = parse("classes/HelloWorld");
	}

	struct ClassFile *cf;
};

TEST_F(ResolveTest, resolveClassShouldReturnRightClass) {
	EXPECT_EQ(cf, resolve_class(cf, 4));
};

TEST_F(ResolveTest, resolveClassShouldResolve) {
	resolve_class(cf, 4);

	EXPECT_EQ(cf, cf->constant_pool[4].r_methodref_info.r_class);
};

// TODO: resolveClassToLoad

TEST_F(ResolveTest, resolveNameAndTypeShouldResolve) {
	struct cp_info *cp = cf->constant_pool;
	resolve_nameAndType(cp, (u2) 16);

	EXPECT_STREQ("<init>", cp[16].r_nameAndType_info.name_str);
	EXPECT_STREQ("()V", cp[16].r_nameAndType_info.signature_str);
};

TEST_F(ResolveTest, resolveMethodrefShouldReturnRight) {
	struct r_methodref_info *m 
		= resolve_methodref(cf, 3);

	// we don't know the actual adress of the r_method_info
	// check name instead
	EXPECT_STREQ("println", m->r_method->name);
};

TEST_F(ResolveTest, resolveMethodrefShouldResolveClass) {
	resolve_methodref(cf, 3);

	EXPECT_EQ(cf, cf->constant_pool[3].r_methodref_info.r_class);
};

TEST_F(ResolveTest, resolveMethodrefShouldResolve) {
	resolve_methodref(cf, 3);

	// we don't know the actual adress of the r_method_info
	// check name instead
	EXPECT_STREQ("println",
		cf->constant_pool[3].r_methodref_info.r_method->name);
};

// TODO: resolveMethodref with different class 
// and methods with same name/signature

// TODO: resolveConstant


