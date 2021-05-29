#include <folly/init/Init.h>
#include <gtest/gtest.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/lib/http/HTTPMessage.h>
#include "../Router.h"

class RouterTest : public ::testing::Test {
public:
    void SetUp() override {
        m_router = std::make_unique<Router>("test");
    }

    void TearDown() override {
        m_router.reset();
    }

    static void found(Router* router, const proxygen::HTTPMessage* msg) {
        std::unique_ptr<proxygen::RequestHandler> handler(router->dispatch(msg));
        ASSERT_FALSE(handler);
    }

    static void notFound(Router* router, const proxygen::HTTPMessage* msg) {
        std::unique_ptr<proxygen::RequestHandler> handler(router->dispatch(msg));
        ASSERT_TRUE(handler);
    }

    void found(const proxygen::HTTPMessage* msg) {
        found(m_router.get(), msg);
    }

    void notFound(const proxygen::HTTPMessage* msg) const {
        notFound(m_router.get(), msg);
    }

private:
    std::unique_ptr<Router> m_router;
};

TEST_F(RouterTest, TestNoPathParams) {
    m_router->get("/foo/bar")
        .handler([](PathParams&& params) -> proxygen::RequestHandler {
            EXPECT_TRUE(params.empty());
            return nullptr;
        });
    
    proxygen::HTTPMessage msg;
    msg.setMethod(proxygen::HTTPMessage::GET);
    msg.setURL("https://localhost/test/foo/bar");
    found(&msg);

    msg.SetURL("https://localhost/test/foo/bar/");
    found(&msg);

     msg.setMethod(proxygen::HTTPMethod::PUT);
    notFound(&msg);

    msg.setMethod(proxygen::HTTPMethod::GET);
    msg.setURL("https://localhost/foo/bar");
    notFound(&msg);

    msg.setURL("https://localhost/test/foo");
    notFound(&msg);

    msg.setURL("https://localhost/test/");
    notFound(&msg);
}

TEST_F(RouterTest, TestPathParams) {
    m_router->get("/foo/:bar/bas")
        .handler([](PathParams&& params) -> proxygen::RequestHandler {
            EXPECT_EQ(1, params.size());
            auto iter = params.find("bar");
            EXPECT_NE(iter, params.end());
            return nullptr;
        });
    
    proxygen::HTTPMessage msg;
    msg.setMethod(proxygen::HTTPMethod::PUT);
    msg.setURL("https://localhost/test/foo/hello/baz");
    notFound(&msg);

    msg.setMethod(proxygen::HTTPMethod::GET);
    found(&msg);

    msg.setURL("https://localhost/test/foo/hello");
    notFound(&msg);

    msg.setURL("https://localhost/foo/hello");
    notFound(&msg);

    msg.setURL("https://localhost/foo/hello/baz");
    notFound(&msg);
}

TEST_F(RouterTest, TestDiffHttpMethod) {
    int getCount = 0;
    m_router->get("/foo/:bar/baz").handler([&getCount](auto&& params) {
        EXPECT_EQ(params.size(), 1);
        getCount++;
        return nullptr;
    });
    int putCount = 0;
    m_router->put("/foo/:bar/baz").handler([&putCount](auto&& params) {
        EXPECT_EQ(params.size(), 1);
        putCount++;
        return nullptr;
    });
    proxygen::HTTPMessage msg;
    msg.setMethod(proxygen::HTTPMethod::PUT);
    msg.setURL("https://localhost/test/foo/hello/baz");
    found(&msg);

    msg.setURL("https://localhost/test/foo/world/baz");
    found(&msg);

    msg.setMethod(proxygen::HTTPMethod::GET);
    msg.setURL("https://localhost/test/foo/hello/baz");
    found(&msg);

    ASSERT_EQ(getCount, 1);
    ASSERT_EQ(putCount, 2);
}

TEST_F(RouterTest, TestNoPrefix) {
    Router router("");
    router.get("/foo/:bar/:baz")
        .handler([](PathParams&& params) -> proxygen::RequestHandler* {
            EXPECT_EQ(params.size(), 2);
            auto iter = params.find("bar");
            EXPECT_NE(iter, params.end());
            iter = params.find("baz");
            EXPECT_NE(iter, params.end());
            return nullptr;
        });

    proxygen::HTTPMessage msg;
    msg.setMethod(proxygen::HTTPMethod::GET);
    msg.setURL("https://localhost/foo/hello/world");
    found(&router, &msg);

    msg.setURL("https://localhost/foo/hello/world/");
    found(&router, &msg);

    msg.setURL("https://localhost/foo/hello");
    notFound(&router, &msg);

    msg.setURL("https://localhost/foo");
    notFound(&router, &msg);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}