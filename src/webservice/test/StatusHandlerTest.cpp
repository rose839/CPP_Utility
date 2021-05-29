#include <gtest/gtest.h>
#include <folly/json.h>
#include "../WebService.h"
#include "common/http/HttpClient.h"

class StatusHandlerTestEnv : public ::testing::Environment {
public:
    void SetUp() override {
        FLAGS_ws_http_port = 0;
        FLAGS_ws_h2_port = 0;

        webSvc_ = std::make_unique<WebService>();
        auto status = webSvc_->start();
        ASSERT_TRUE(status.ok()) << status;
    }

    void TearDown() override {
        webSvc_.reset();
    }
protected:
    std::unique_ptr<WebService> webSvc_;
};


TEST(StatusHandlerTest, SimpleTest) {
    {
        auto url = "/status";
        auto request = folly::stringPrintf("http://%s:%d%s", FLAGS_ws_ip.c_str(),
                                           FLAGS_ws_http_port, url);
        auto resp = http::HttpClient::get(request);
        ASSERT_TRUE(resp.ok());
        auto json = folly::parseJson(resp.value());
        ASSERT_EQ("running", json["status"].asString());
    }
    {
        auto url = "";
        auto request = folly::stringPrintf("http://%s:%d%s", FLAGS_ws_ip.c_str(),
                                           FLAGS_ws_http_port, url);
        auto resp = http::HttpClient::get(request);
        ASSERT_TRUE(resp.ok());
        auto json = folly::parseJson(resp.value());
        ASSERT_EQ("running", json["status"].asString());
    }
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);
    ::testing::AddGlobalTestEnvironment(new nebula::StatusHandlerTestEnv());
    return RUN_ALL_TESTS();
}
