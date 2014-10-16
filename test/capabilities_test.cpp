#include <webdriverxx/capabilities.h>
#include <gtest/gtest.h>

namespace test {

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

TEST(Capabilities, ConvertibleToJson) {
	Capabilities c;
	c.Set("int", 123);
	c.Set("string", "abc");
	const auto json = ToJson(c);
	const auto c_copy = FromJson<Capabilities>(json);
	ASSERT_EQ(123, c_copy.Get<int>("int"));
	ASSERT_EQ("abc", c_copy.Get<std::string>("string"));
}

TEST(Capabilities, AllowsToSetProxy) {
	Capabilities().SetProxy(DirectConnection());
	Capabilities().SetProxy(AutodetectProxy());
	Capabilities().SetProxy(SystemProxy());
	Capabilities().SetProxy(FtpProxy("127.0.0.1:3128").SetNoProxyFor("custom.host"));
	Capabilities().SetProxy(HttpProxy("127.0.0.1:3128").SetNoProxyFor("custom.host"));
	Capabilities().SetProxy(SslProxy("127.0.0.1:3128").SetNoProxyFor("custom.host"));
	Capabilities().SetProxy(SocksProxy("127.0.0.1:3128")
		.SetUsername("user").SetPassword("12345").SetNoProxyFor("custom.host")
		);
	Capabilities().SetProxy(AutomaticProxyFromUrl("http://some.url"));
}

TEST(Capabilities, ConvertsProxyToJson) {
	Capabilities c;
	c.SetProxy(SocksProxy("127.0.0.1:3128").SetUsername("user")
		.SetPassword("12345").SetNoProxyFor("custom.host"));
	const auto json = ToJson(c);
	const auto c_copy = FromJson<Capabilities>(json);
	const auto proxy = FromJson<JsonObject>(c_copy.Get<picojson::value>("proxy"));
	ASSERT_EQ("manual", proxy.Get<std::string>("proxyType"));
	ASSERT_EQ("127.0.0.1:3128", proxy.Get<std::string>("socksProxy"));
	ASSERT_EQ("user", proxy.Get<std::string>("socksUsername"));
	ASSERT_EQ("12345", proxy.Get<std::string>("socksPassword"));
	ASSERT_EQ("custom.host", proxy.Get<std::string>("noProxy"));
}

} // namespace test
