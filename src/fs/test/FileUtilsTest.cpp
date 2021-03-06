#include <gmock/gmock.h>
#include "../FileUtils.h"

TEST(FileUtils, dirname) {
    {
        auto dirname = FileUtils::dirname("/root/test.txt");
        ASSERT_TRUE((dirname == "/root"));
    }
    {
        auto dirname = FileUtils::dirname("/root/");
        ASSERT_TRUE((dirname == "/"));
    }
    {
        auto dirname = FileUtils::dirname("test.txt");
        ASSERT_TRUE((dirname == "."));
    }
}

TEST(FileUtils, basename) {
    {
        auto basename = FileUtils::basename("/root/test.txt");
        ASSERT_TRUE((basename == "test.txt"));
    }
    {
        auto basename = FileUtils::basename("/root/");
        ASSERT_TRUE((basename == "root"));
    }
    {
        auto basename = FileUtils::basename("test.txt");
        ASSERT_TRUE((basename == "test.txt"));
    }
}

TEST(FileUtils, fileType) {
    {
        auto fileType = FileUtils::fileType("/tmp");
        ASSERT_TRUE((fileType == FileType::DIRECTORY));
    }
    {
        auto fileType = FileUtils::fileType("/dev/tty");
        ASSERT_TRUE((fileType == FileType::CHAR_DEV || fileType == FileType::NOTEXIST));
    }
}

int main(int argc, char **argv) {
	testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}