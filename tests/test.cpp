#include <gtest/gtest.h>
#include <nx/alias.h>

TEST(SampleTest, AssertionTrue) { EXPECT_TRUE(true); }
TEST(file_system, test) {  
	auto result = nx::fs::read_file("/no_such_file");
	EXPECT_EQ(std::get<nx::IO_Error>(result), nx::IO_Error::NOT_OPEN);
}
