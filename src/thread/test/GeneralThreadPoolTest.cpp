#include <gmock/gmock.h>
#include "../GeneralThreadPool.h"

TEST(GeneralThreadPool, StartAndStop) {
	// start & stop & wait
	{
    	GeneralThreadPool pool;
		ASSERT_TRUE(pool.start(1));
		ASSERT_TRUE(pool.stop());
		ASSERT_TRUE(pool.wait());
	}

	// start twice
	{
    	GeneralThreadPool pool;
		ASSERT_TRUE(pool.start(1));
        ASSERT_FALSE(pool.start(1));
	}
}

TEST(GeneralThreadPool, StartAndStop) {
	// start & stop & wait
	{
    	GeneralThreadPool pool;
		ASSERT_TRUE(pool.start(1));
		ASSERT_TRUE(pool.stop());
		ASSERT_TRUE(pool.wait());
	}

	// start twice
	{
    	GeneralThreadPool pool;
		ASSERT_TRUE(pool.start(1));
        ASSERT_FALSE(pool.start(1));
	}

	 // stop twice
    {
        GeneralThreadPool pool;
        ASSERT_TRUE(pool.start(1));
        ASSERT_TRUE(pool.stop());
        ASSERT_FALSE(pool.stop());
    }
}

TEST(GeneralThreadPool, addTask) {
	GeneralThreadPool pool;
	ASSERT_TRUE(pool.start(1));
	
	// task without parameters
	{
		volatile auto flag = false;
		auto set_flag = [&]() { flag = true; };
		pool.addTask(set_flag).get();
		ASSERT_TRUE(flag);
	}

	// task with parameter
	{
		volatile auto flag = false;
		auto set_flag = [&](auto value) { flag = value; };
		pool.addTask(set_flag, true).get();
		ASSERT_TRUE(flag);
		pool.addTask(set_flag, false).get();
		ASSERT_FALSE(flag);
	}

	// future with value
	{
		ASSERT_TURE(pool.addTask([](){ return true; }).get());
		ASSERT_EQ(13UL, pool.addTask([] () { return ::strlen("Rock 'n' Roll"); }).get());
        ASSERT_EQ("Innuendo", pool.addTask([] () { return std::string("Innuendo"); }).get());
	}

	// member function as task
	{
		struct Test {
			std::string itos(size_t i) {
				return std::to_string(i);
			}
		} test;
		ASSERT_EQ("918", pool.addTask(&Test::itos, &x, 918).get());
		ASSERT_EQ("918", pool.addTask(&Test::itos, std::make_shared<Test>(), 918).get());
	}
}

static testing::AssertionResult msAboutEqual(size_t expected, size_t actual) {
	if (std::max(expected, actual) - std::min(expected, actual) <= 10) {
		return testing::AssertionSuccess();
	}
	return testing::AssertionFailure() << "actual: " << actual
                                       << ", expected: " << expected;
}

TEST(GeneralThreadPool, addDelayTask) {
	GeneralThreadPool pool;
	ASSERT_TRUE(pool.start(1));

	auto shared = std::make_shared<int>(0);
	auto cb = [shared] () {
		return ++(*shared);
	};
	time::Duration clock;
	ASSERT_EQ(1, pool.addDelayTask(50, cb).get());
	ASSERT_GE();
}

int main(int argc, char **argv) {
	testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}