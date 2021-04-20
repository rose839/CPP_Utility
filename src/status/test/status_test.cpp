#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include <gmock/gmock.h>
#include "../status.h"

TEST(Status, CheckErrorCodeOk) {
    Status ok;
    if (ok.code() != Status::kOk) {
        ASSERT_TRUE(false);
    }
}

TEST(Status, CheckErrorCodeError) {
	Status error = Status::Error();
    if (!error.isError()) {
        ASSERT_TRUE(false);
    }

    if (error.toString() != "GeneralError: ") {
        ASSERT_TRUE(false);
    }
}

TEST(Status, CheckErrorMsg) {
	Status error = Status::NoSuchFile("not found file %s", "helloworld.txt");
    if (!error.isNoSuchFile()) {
        ASSERT_TRUE(false);
    }

    if (error.toString() != "NoSuchFile: not found file helloworld.txt") {
        ASSERT_TRUE(false);
    }

    if (error.message() != "not found file helloworld.txt") {
        ASSERT_TRUE(false);
    }
}

int main(int argc, char **argv) {
	testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}