#include <gtest/gtest.h>

extern "C" {
	#include "../src/parser.h"
}


class ParserTest : public ::testing::Test {
	protected:
	static void SetUpTestCase() {
		cf = parse("classes/HelloWorld.class");
	}
	
	static ClassFile *cf;
};

ClassFile *ParserTest::cf = NULL;

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
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[6].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[7].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[8].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[9].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[10].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[11].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[12].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[13].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[14].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[15].tag);
	EXPECT_EQ(CONSTANT_NameAndType, cf->constant_pool[16].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[17].tag);
	EXPECT_EQ(CONSTANT_NameAndType, cf->constant_pool[18].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[19].tag);
	EXPECT_EQ(CONSTANT_Utf8, cf->constant_pool[20].tag);
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

TEST_F(ParserTest, Utf8Constant) {
	struct cp_info cp6 = ParserTest::cf->constant_pool[6];
	ASSERT_EQ(CONSTANT_Utf8, cp6.tag);
	EXPECT_EQ(6, cp6.utf8_info.length);
	EXPECT_STREQ("<init>", (const char *) cp6.utf8_info.bytes);
}

TEST_F(ParserTest, NameAndTypeConstant) {
	struct cp_info cp16 = ParserTest::cf->constant_pool[16];
	ASSERT_EQ(CONSTANT_NameAndType, cp16.tag);
	EXPECT_EQ(6, cp16.nameAndType_info.name_index);
	EXPECT_EQ(7, cp16.nameAndType_info.signature_index);
}


TEST_F(ParserTest, NoInterfaces) {
	EXPECT_EQ(0, ParserTest::cf->interfaces_count);
}


TEST_F(ParserTest, NoFields) {
	EXPECT_EQ(0, ParserTest::cf->fields_count);
}


TEST_F(ParserTest, ConstructorMethod) {
	struct method_info m0 = ParserTest::cf->methods[0];
	EXPECT_EQ(ACC_PUBLIC, m0.access_flags);
	EXPECT_EQ(6, m0.name_index);
	EXPECT_EQ(7, m0.signature_index);
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

}

TEST_F(ParserTest, SingleSourcecodeAttribute) {
	EXPECT_EQ(1, ParserTest::cf->attributes_count);
	EXPECT_EQ(14, ParserTest::cf->attributes[0].attribute_name_index);
}

