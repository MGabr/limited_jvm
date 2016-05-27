#include <gtest/gtest.h>

extern "C" {
	#include "../src/string_pool.h"
	#include "../src/options.h"
}


class StringPoolConflictTest : public ::testing::Test {
	protected:
	virtual void setUp() {
		nr_string_buckets = 1;
		init_string_pool();
	}
	/* string pool does not provide a function 
	 * to free the memory for the underlying hash table,
	 * since it is not needed by the vm
	 *
	 * we are intentionally risking memory leak 
	 */
};


TEST_F(StringPoolConflictTest, validStringAdress) {
//        add_string("stringToAdd");
//        EXPECT_STREQ("conflictingString", add_string("conflictingString"));
}

/*
TEST_F(StringPoolConflictTest, sameFirstStringAdress) {
	const char *str = add_string("sameString");
	add_string("conflictingString");
	EXPECT_EQ(str, add_string("sameString"));
}

TEST_F(StringPoolConflictTest, sameSecondStringAdress) {
        add_string("conflictingString");
        const char *str = add_string("sameString");
        EXPECT_EQ(str, add_string("sameString"));
}

TEST_F(StringPoolConflictTest, sameFirstFindStringAdress) {
	const char *str = add_string("stringToFind");
	add_string("conflictingString");
	EXPECT_EQ(str, find_string("stringToFind"));
}

TEST_F(StringPoolConflictTest, sameSecondFindStringAdress) {
	add_string("conflictingString");
	const char *str = add_string("stringToFind");
	EXPECT_EQ(str, find_string("stringToFind"));
}
*/

