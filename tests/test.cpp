#include <gtest/gtest.h>
#include <nx/alias.h>

TEST(SampleTest, AssertionTrue) { EXPECT_TRUE(true); }

TEST(file_system, test) {  
	auto result = nx::fs::read_file("/no_such_file");
	EXPECT_EQ(std::get<nx::IO_Error>(result), nx::IO_Error::NOT_OPEN);
}

TEST(math, test) {  
	EXPECT_EQ(nx::ceil_pow2(0u), 0);
	EXPECT_EQ(nx::ceil_pow2(1u), 1);
	EXPECT_EQ(nx::ceil_pow2(2u), 2);
	EXPECT_EQ(nx::ceil_pow2(3u), 4);
	EXPECT_EQ(nx::ceil_pow2(4u), 4);
	EXPECT_EQ(nx::ceil_pow2(7u), 8);
	EXPECT_EQ(nx::ceil_pow2(1024u), 1024);
	EXPECT_EQ(nx::ceil_pow2(1025u), 2048);
}
