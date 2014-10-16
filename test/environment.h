#ifndef WEBDRIVERXX_ENVIRONMENT_H
#define WEBDRIVERXX_ENVIRONMENT_H

#include <webdriverxx/webdriver.h>
#include <webdriverxx/capabilities.h>
#include <gtest/gtest.h>
#include <string>
#include <algorithm>

namespace test {

const char* const kDefaultTestWebDriverUrl = "http://localhost:7777/";
const char* const kDefaultTestPagesUrl = "http://localhost:8080/";

struct Parameters {
	std::string web_driver_url;
	webdriverxx::Capabilities required;
	webdriverxx::Capabilities desired;
	std::string test_pages_url;
	bool test_real_browsers;

	Parameters()
		: web_driver_url(kDefaultTestWebDriverUrl)
		, test_pages_url(kDefaultTestPagesUrl)
		, test_real_browsers(false)
	{}
};

class Environment : public ::testing::Environment {
public:
	static Environment& Instance() {
		return *instance_;
	}

	explicit Environment(const Parameters& parameters)
		: driver_(0)
		, parameters_(parameters)
	{}

	webdriverxx::WebDriver& GetDriver() {
		return driver_ ? *driver_ : GetFreshDriver();
	}

	webdriverxx::WebDriver& GetFreshDriver() {
		DeleteDriver();
		driver_ = new webdriverxx::WebDriver(CreateDriver());
		return *driver_;
	}

	webdriverxx::WebDriver CreateDriver() {
		return webdriverxx::WebDriver(
			parameters_.desired,
			parameters_.required,
			parameters_.web_driver_url
			);
	}

	std::string GetWebDriverUrl() const { return parameters_.web_driver_url; }

	Parameters GetParameters() const { return parameters_; }

	std::string GetTestPageUrl(const std::string& page_name) const {
		std::string url = parameters_.test_pages_url;
		if (!url.empty() && url[url.length() - 1] != '/')
			url += "/";
		url += page_name;
		return url;
	}

private:
	void SetUp() {
		instance_ = this;
	}

	void TearDown() {
		instance_ = 0;
		DeleteDriver();
	}

	void DeleteDriver() {
		delete driver_;
		driver_ = 0;
	}

private:
	static Environment* instance_;
	webdriverxx::WebDriver* driver_;
	Parameters parameters_;
};

inline Parameters GetParameters() { return Environment::Instance().GetParameters(); }
inline std::string GetWebDriverUrl() { return Environment::Instance().GetWebDriverUrl(); }
inline std::string GetTestPageUrl(const std::string& page_name) { return Environment::Instance().GetTestPageUrl(page_name); }
inline webdriverxx::WebDriver& GetDriver() { return Environment::Instance().GetDriver(); }
inline webdriverxx::WebDriver& GetFreshDriver() { return Environment::Instance().GetFreshDriver(); }
inline webdriverxx::WebDriver CreateDriver() { return Environment::Instance().CreateDriver(); }
inline bool TestRealBrowsers() { return GetParameters().test_real_browsers; }

} // namespace test

#endif
