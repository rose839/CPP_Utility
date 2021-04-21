#include <gmock/gmock.h>
#include "../ProcessUtils.h"

TEST(ProcessUtils, isPidAvailable) {
    {
        auto status = ProcessUtils::isPidAvailable(::getpid());
        ASSERT_TRUE(status.ok());
    }

    {
        auto status = ProcessUtils::isPidAvailable(0);
        ASSERT_FALSE(status.ok());
    }

    {
        auto status = ProcessUtils::isPidAvailable(1);
        ASSERT_FALSE(status.ok());
    }
}

int main(int argc, char **argv) {
	testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}