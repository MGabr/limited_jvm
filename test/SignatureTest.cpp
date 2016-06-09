#include <gtest/gtest.h>

extern "C" {
	#include "../src/signature.h"
}

TEST(SignatureTest, NoArgsShouldCalculate0) {
	EXPECT_EQ(0, calculate_nargs("()V"));
}

TEST(SignatureTest, ByteArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("(B)V"));
}

TEST(SignatureTest, CharArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("(C)V"));
}

TEST(SignatureTest, FloatArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("(F)V"));
}

TEST(SignatureTest, IntArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("(I)V"));
}

TEST(SignatureTest, ShortArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("(S)V"));
}

TEST(SignatureTest, BooleanArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("(Z)V"));
}

TEST(SignatureTest, LongArgShouldCalculate2) {
	EXPECT_EQ(2, calculate_nargs("(J)V"));
}

TEST(SignatureTest, DoubleArgShouldCalculate2) {
	EXPECT_EQ(2, calculate_nargs("(D)V"));
}

TEST(SignatureTest, ClassArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("(Lorg/something/Example;)V"));
}

TEST(SignatureTest, ArrayArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("([B)V"));
}

TEST(SignatureTest, ArrayOfClassesArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("([Lorg/something/Example;)V"));
}

TEST(SignatureTest, ArrayOfArraysArgShouldCalculate1) {
	EXPECT_EQ(1, calculate_nargs("([[B)V"));
}

TEST(SignatureTest, ComplexSignature) {
	EXPECT_EQ(8, calculate_nargs("(BF[[Lorg/something/BCFISZLJD;JLorg/A;D)V"));
}


