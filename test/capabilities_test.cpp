#include <webdriverxx/capabilities.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

TEST(Capabilities, AllowsToSetAndGetCustomValues) {
	Capabilities c;
	c.Set("int", 123);
	c.Set("double", 456.7);
	c.Set("string", "abc");
	c.Set("bool", true);
	ASSERT_EQ(123, c.Get<int>("int"));
	ASSERT_EQ(456.7, c.Get<double>("double"));
	ASSERT_EQ("abc", c.Get<std::string>("string"));
	ASSERT_EQ(true, c.Get<bool>("bool"));
}
