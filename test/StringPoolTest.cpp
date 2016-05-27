#include <gtest/gtest.h>

extern "C" {
	#include "../src/string_pool.h"
}


class StringPoolTest : public ::testing::Test {
	protected:
	virtual void SetUp() {
		init_string_pool();
	}
	/* string pool does not provide a function 
	 * to free the memory for the underlying hash table,
	 * since it is not needed by the vm
	 *
	 * we are intentionally allocating new memory every time 
	 */
};

TEST_F(StringPoolTest, doNotCopyString) {
	const char *str = "string";
	EXPECT_EQ(str, add_string(str));
}

TEST_F(StringPoolTest, validStringAdress) {
	EXPECT_STREQ("stringToAdd", add_string("stringToAdd"));
}

TEST_F(StringPoolTest, sameStringAdress){
	const char *str = add_string("sameString");
	EXPECT_EQ(str, add_string("sameString"));
}

TEST_F(StringPoolTest, sameFindStringAdress) {
	const char *str = add_string("stringToFind");
	EXPECT_EQ(str, find_string("stringToFind"));
}

TEST_F(StringPoolTest, notExistingFindString) {
	EXPECT_EQ(NULL, find_string("notExistingString"));
}

