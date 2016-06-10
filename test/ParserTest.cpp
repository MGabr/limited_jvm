#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
	#include "../src/string_pool.h"
}


class ParserTest : public ::testing::Test {
	protected:
	static void SetUpTestCase() {
		cf = parse("classes/HelloWorld");
	}
	
	static struct ClassFile *cf;
};

struct ClassFile *ParserTest::cf = NULL;

TEST_F(ParserTest, Header) {
	EXPECT_EQ(0xcafebabe, ParserTest::cf->magic);
	EXPECT_EQ(0, ParserTest::cf->minor_version);
	EXPECT_EQ(52, ParserTest::cf->major_version);
}


TEST_F(ParserTest, ConstantPoolCount) {
	EXPECT_EQ(21, ParserTest::cf->constant_pool_count);
}

TEST_F(ParserTest, ConstantPoolTags) {
	struct ClassFile *cf = ParserTest::cf;
	EXPECT_EQ(CONSTANT_Methodref, cf->constant_pool[1].tag);
	EXPECT_EQ(CONSTANT_String, cf->constant_pool[2].tag);
	EXPECT_EQ(CONSTANT_Methodref, cf->constant_pool[3].tag);
	EXPECT_EQ(CONSTANT_Class, cf->constant_pool[4].tag);
	EXPECT_EQ(CONSTANT_Class, cf->constant_pool[5].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[6].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[7].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[8].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[9].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[10].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[11].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[12].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[13].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[14].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[15].tag);
	EXPECT_EQ(CONSTANT_NameAndType, cf->constant_pool[16].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[17].tag);
	EXPECT_EQ(CONSTANT_NameAndType, cf->constant_pool[18].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[19].tag);
	EXPECT_EQ(RESOLVED_Utf8, cf->constant_pool[20].tag);
}

TEST_F(ParserTest, MethodrefConstant) {
	struct cp_info cp1 = ParserTest::cf->constant_pool[1];
	ASSERT_EQ(CONSTANT_Methodref, cp1.tag);
	EXPECT_EQ(5, cp1.methodref_info.class_index);
	EXPECT_EQ(16, cp1.methodref_info.name_and_type_index);
}

TEST_F(ParserTest, StringConstant) {
	struct cp_info cp2 = ParserTest::cf->constant_pool[2];
	ASSERT_EQ(CONSTANT_String, cp2.tag);
	EXPECT_EQ(17, cp2.string_info.string_index);
}

TEST_F(ParserTest, ClassConstant) {
	struct cp_info cp4 = ParserTest::cf->constant_pool[4];
	ASSERT_EQ(CONSTANT_Class, cp4.tag);
	EXPECT_EQ(19, cp4.class_info.name_index);
}

TEST_F(ParserTest, ResolvedUtf8Constant) {
	struct cp_info cp6 = ParserTest::cf->constant_pool[6];
	ASSERT_EQ(RESOLVED_Utf8, cp6.tag);
	EXPECT_STREQ("<init>", cp6.r_utf8_info.str);
}

// Tests if the Utf8 string was saved in the string pool
TEST_F(ParserTest, Utf8InStringPool) {
	struct cp_info cp6 = ParserTest::cf->constant_pool[6];
	EXPECT_EQ(cp6.r_utf8_info.str, find_string("<init>"));
}

TEST_F(ParserTest, NameAndTypeConstant) {
	struct cp_info cp16 = ParserTest::cf->constant_pool[16];
	ASSERT_EQ(CONSTANT_NameAndType, cp16.tag);
	EXPECT_EQ(6, cp16.nameAndType_info.name_index);
	EXPECT_EQ(7, cp16.nameAndType_info.signature_index);
	EXPECT_STREQ("()V", ParserTest::cf->constant_pool[cp16.nameAndType_info.signature_index].r_utf8_info.str);
}


TEST_F(ParserTest, NoInterfaces) {
	EXPECT_EQ(0, ParserTest::cf->interfaces_count);
}


TEST_F(ParserTest, NoFields) {
	EXPECT_EQ(0, ParserTest::cf->fields_count);
}


TEST_F(ParserTest, ConstructorMethod) {
	struct r_method_info m0 = ParserTest::cf->methods[0];
	EXPECT_EQ(ACC_PUBLIC, m0.access_flags);
	EXPECT_STREQ("<init>", m0.name);
	EXPECT_STREQ("()V", m0.signature);
	EXPECT_EQ(0, m0.nargs);
	EXPECT_EQ(1, m0.attributes_count);
}

TEST_F(ParserTest, ConstructorMethodCode) {
	struct attribute_info a0 = ParserTest::cf->methods[0].attributes[0];
	ASSERT_EQ(8, a0.attribute_name_index);
	EXPECT_EQ(29, a0.attribute_length);
	struct Code_attribute c0 = a0.code_attribute;
	EXPECT_EQ(1, c0.max_stack);
	EXPECT_EQ(1, c0.max_locals);
	EXPECT_EQ(5, c0.code_length);
	EXPECT_EQ(0, c0.exception_table_length);
	EXPECT_EQ(1, c0.attributes_count);
}

TEST_F(ParserTest, ConstructorMethodInstructions) {
	u1 *code = ParserTest::cf->methods[0].attributes[0].code_attribute.code;
	EXPECT_EQ(42, *code); // aload_0
//	EXPECT_EQ(183, code[1]); // invokenonvirtual
//	EXPECT_EQ(0, code[2]); // index byte 1
//	EXPECT_EQ(1, code[3]); // index byte 2
//	EXPECT_EQ(177, code[4]); // return
}

TEST_F(ParserTest, SingleSourcecodeAttribute) {
	EXPECT_EQ(1, ParserTest::cf->attributes_count);
	EXPECT_EQ(14, ParserTest::cf->attributes[0].attribute_name_index);
}

TEST_F(ParserTest, Name) {
	EXPECT_STREQ("HelloWorld", ParserTest::cf->name);
}

TEST_F(ParserTest, NextShouldPointToItself) {
	EXPECT_EQ(ParserTest::cf, ParserTest::cf->next);
}

TEST_F(ParserTest, ConstructorMethodCAttr) {
	// first attribute is code attribute
	struct Code_attribute *c_attr 
		= &ParserTest::cf->methods[0].attributes[0].code_attribute;

	EXPECT_EQ(c_attr, ParserTest::cf->methods[0].c_attr);
}

