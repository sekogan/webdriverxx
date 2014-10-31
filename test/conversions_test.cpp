#include <webdriverxx/conversions.h>
#include <gtest/gtest.h>
#include <vector>
#include <list>

namespace test {

using namespace webdriverxx;

TEST(ToJson, ConvertsIntegralTypes) {
	ASSERT_EQ(123, ToJson(123).get<double>());
	int i = 123;
	ASSERT_EQ(123, ToJson(i).get<double>());
	ASSERT_EQ(123, ToJson(static_cast<const int>(123)).get<double>());
	ASSERT_EQ(123.5, ToJson(123.5).get<double>());
	ASSERT_TRUE(ToJson(true).get<bool>());
	ASSERT_FALSE(ToJson(false).get<bool>());
}

TEST(ToJson, ConvertsStrings) {
	ASSERT_EQ("abc", ToJson("abc").get<std::string>());
	std::string s("abc");
	ASSERT_EQ("abc", ToJson(s).get<std::string>());
	ASSERT_EQ("abc", ToJson(s.c_str()).get<std::string>());
	ASSERT_EQ("abc", ToJson(const_cast<char*>(s.c_str())).get<std::string>());
	char a[] = "abc";
	ASSERT_EQ("abc", ToJson(a).get<std::string>());
}

TEST(ToJson, ConvertsIterables) {
	int i[] = { 123, 456, 789 };
	const auto ji = ToJson(i);
	ASSERT_TRUE(ji.is<picojson::array>());
	ASSERT_EQ(3u, ji.get<picojson::array>().size());
	ASSERT_EQ(789, ji.get<picojson::array>()[2].get<double>());

	const char* s[] = { "abc", "def", "ghi" };
	const auto js = ToJson(s);
	ASSERT_TRUE(js.is<picojson::array>());
	ASSERT_EQ(3u, js.get<picojson::array>().size());
	ASSERT_EQ("ghi", js.get<picojson::array>()[2].get<std::string>());

	std::vector<int> v(std::begin(i), std::end(i));
	const auto jv = ToJson(v);
	ASSERT_TRUE(jv.is<picojson::array>());
	ASSERT_EQ(3u, jv.get<picojson::array>().size());
	ASSERT_EQ(789, jv.get<picojson::array>()[2].get<double>());

	std::list<int> l(std::begin(i), std::end(i));
	const auto jl = ToJson(l);
	ASSERT_TRUE(jl.is<picojson::array>());
	ASSERT_EQ(3u, jl.get<picojson::array>().size());
	ASSERT_EQ(789, jl.get<picojson::array>()[2].get<double>());

	int ii[2][3] = { { 1, 2, 3 }, { 4, 5, 123 } };
	const auto jii = ToJson(ii);
	ASSERT_TRUE(jii.is<picojson::array>());
	ASSERT_EQ(2u, jii.get<picojson::array>().size());
	ASSERT_TRUE(jii.get<picojson::array>()[0].is<picojson::array>());
	ASSERT_TRUE(jii.get<picojson::array>()[1].is<picojson::array>());
	ASSERT_EQ(3u, jii.get<picojson::array>()[0].get<picojson::array>().size());
	ASSERT_EQ(3u, jii.get<picojson::array>()[1].get<picojson::array>().size());
	ASSERT_EQ(123, jii.get<picojson::array>()[1].get<picojson::array>()[2].get<double>());

	std::vector<int> e;
	const auto je = ToJson(e);
	ASSERT_TRUE(je.is<picojson::array>());
	ASSERT_EQ(0u, je.get<picojson::array>().size());
}

namespace {
namespace custom {

struct Object {
	std::string string;
	int number;
};

picojson::value CustomToJson(const Object& value) {
	return JsonObject()
		.Set("string", value.string)
		.Set("number", value.number);
}

void CustomFromJson(const picojson::value& value, Object& result) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "custom::Object is not an object");
	result.string = FromJson<std::string>(value.get("string"));
	result.number = FromJson<int>(value.get("number"));
}

} // namespace custom
} // namespace

TEST(ToJson, ConvertsCustomObjects) {
	custom::Object o = { "abc", 123 };
	const auto jo = ToJson(o);
	ASSERT_TRUE(jo.is<picojson::object>());
	ASSERT_EQ(123, jo.get("number").get<double>());
	ASSERT_EQ("abc", jo.get("string").get<std::string>());

	const custom::Object os[] = { { "abc", 123 }, { "def", 456 } };
	const auto jos = ToJson(os);
	ASSERT_TRUE(jos.is<picojson::array>());
	ASSERT_EQ(2u, jos.get<picojson::array>().size());
	ASSERT_TRUE(jos.get<picojson::array>()[0].is<picojson::object>());
	ASSERT_EQ(456, jos.get<picojson::array>()[1].get("number").get<double>());
}

picojson::value J(const std::string& json) {
	picojson::value result;
	picojson::parse(result, json.begin(), json.end(), nullptr);
	return result;
}

TEST(FromJson, ConvertsIntegralTypes) {
	ASSERT_EQ(123, FromJson<int>(J("123")));
	ASSERT_EQ(123.5, FromJson<double>(J("123.5")));
	ASSERT_EQ(123u, FromJson<unsigned>(J("123")));
	ASSERT_FALSE(FromJson<bool>(J("false")));
	ASSERT_TRUE(FromJson<bool>(J("true")));
}

TEST(FromJson, ConvertsStrings) {
	ASSERT_EQ("abc", FromJson<std::string>(J("\"abc\"")));
}

TEST(FromJson, ConvertsIterables) {
	const auto i = FromJson<std::vector<int>>(J("[ 123, 456, 789 ]"));
	ASSERT_EQ(3u, i.size());
	ASSERT_EQ(123, i[0]);
	ASSERT_EQ(789, i[2]);

	const auto s = FromJson<std::list<std::string>>(J("[ \"abc\", \"def\" ]"));
	ASSERT_EQ(2u, s.size());
	ASSERT_EQ("abc", s.front());
	ASSERT_EQ("def", s.back());

	const auto e = FromJson<std::vector<int>>(J("[]"));
	ASSERT_EQ(0u, e.size());

	const auto ii = FromJson<std::vector<std::list<int>>>(J("[ [ 123, 456 ], [ 789 ] ]"));
	ASSERT_EQ(2u, ii.size());
	ASSERT_EQ(2u, ii[0].size());
	ASSERT_EQ(1u, ii[1].size());
	ASSERT_EQ(456, ii[0].back());
	ASSERT_EQ(789, ii[1].front());
}

TEST(FromJson, ConvertsCustomObjects) {
	const auto o = FromJson<custom::Object>(J("{ \"number\": 123, \"string\": \"abc\" }"));
	ASSERT_EQ(123, o.number);
	ASSERT_EQ("abc", o.string);

	const auto os = FromJson<std::vector<custom::Object>>(J(
		"[ { \"number\": 123, \"string\": \"abc\" },"
		"{ \"number\": 456, \"string\": \"def\" } ]"
		));
	ASSERT_EQ(2u, os.size());
	ASSERT_EQ(123, os[0].number);
	ASSERT_EQ("abc", os[0].string);
	ASSERT_EQ(456, os[1].number);
	ASSERT_EQ("def", os[1].string);
}

} // namespace test
