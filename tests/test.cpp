#include <gtest/gtest.h>
#include <nx/alias.h>

TEST(SampleTest, AssertionTrue) { EXPECT_TRUE(true); }

TEST(file_system, read_file) {  
	auto result = nx::fs::read_file("/no_such_file");
	EXPECT_EQ(std::get<nx::IO_Error>(result), nx::IO_Error::NOT_OPEN);
}

TEST(math, ceil_pow2) {  
	EXPECT_EQ(nx::ceil_pow2(0u), 0);
	EXPECT_EQ(nx::ceil_pow2(1u), 1);
	EXPECT_EQ(nx::ceil_pow2(2u), 2);
	EXPECT_EQ(nx::ceil_pow2(3u), 4);
	EXPECT_EQ(nx::ceil_pow2(4u), 4);
	EXPECT_EQ(nx::ceil_pow2(7u), 8);
	EXPECT_EQ(nx::ceil_pow2(1024u), 1024);
	EXPECT_EQ(nx::ceil_pow2(1025u), 2048);
}

TEST(digest, md5) {  
	EXPECT_EQ(nx::md5("hello, world\n"), "22c3683b094136c3398391ae71b20f04");
}

TEST(digest, sha256)
{
    EXPECT_EQ(
        nx::sha256("hello, world\n"),
        "853ff93762a06ddbf722c4ebe9ddd66d8f63ddaea97f521c3ecc20da7c976020");
}

TEST(digest, crc32) { EXPECT_EQ(nx::crc32("hello, world"), 0xffab723a); }

TEST(file_system, archive)
{
    {
        auto archive = nx::fs::create_archive("invalid:///path");
        EXPECT_EQ(archive, nullptr);
    }

    {
        auto archive = nx::fs::create_archive("dir:///");
        EXPECT_TRUE(archive != nullptr);
        EXPECT_TRUE(archive->list_dir("/__invalid__").size() == 0);
        EXPECT_TRUE(archive->list_dir("/").size() > 0);
        EXPECT_TRUE(archive->open("/dev/null") != nullptr);
    }
}