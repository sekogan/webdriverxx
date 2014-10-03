#ifndef WEBDRIVERXX_ENVIRONMENT_H
#define WEBDRIVERXX_ENVIRONMENT_H

#include <webdriverxx/webdriver.h>
#include <webdriverxx/capabilities.h>
#include <gtest/gtest.h>
#include <string>
#include <algorithm>

const char* const kDefaultUrl = "http://localhost:7777/";
const char* const kDefaultPagesUrl = "http://localhost:8080/";

struct Parameters {
	std::string url;
	webdriverxx::Capabilities required;
	webdriverxx::Capabilities desired;
	std::string pages_url;

	Parameters()
		: url(kDefaultUrl)
		, pages_url(kDefaultPagesUrl)
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

	webdriverxx::WebDriver* GetDriver() {
		return driver_ ? driver_ : GetFreshDriver();
	}

	webdriverxx::WebDriver* GetFreshDriver() {
		DeleteDriver();
		return driver_ = new webdriverxx::WebDriver(
			parameters_.url, parameters_.desired, parameters_.required
			);
	}

	std::string GetUrl() const {
		return parameters_.url;
	}

	Parameters GetParameters() const {
		return parameters_;
	}

	std::string GetTestPageUrl(const std::string& page_name) const {
		std::string url = parameters_.pages_url;
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

#endif
