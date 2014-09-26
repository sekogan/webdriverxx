#ifndef WEBDRIVERXX_ENVIRONMENT_H
#define WEBDRIVERXX_ENVIRONMENT_H

#include <webdriverxx/webdriver.h>
#include <webdriverxx/capabilities.h>
#include <gtest/gtest.h>
#include <string>

const char* const kPhantomUrl = "http://localhost:7777/";

struct DriverParameters
{
	std::string url;
	webdriverxx::Capabilities required;
	webdriverxx::Capabilities desired;
};

class Environment : public ::testing::Environment {
public:
	static Environment& Instance() {
		return *instance_;
	}

	explicit Environment(const DriverParameters& parameters)
		: driver_(0)
		, parameters_(parameters) {}

	webdriverxx::WebDriver* GetDriver() {
		return driver_ ? driver_ : GetFreshDriver();
	}

	webdriverxx::WebDriver* GetFreshDriver() {
		DeleteDriver();
		return driver_ = new webdriverxx::WebDriver(
			parameters_.url, parameters_.required, parameters_.desired
			);
	}

	std::string GetUrl() const {
		return parameters_.url;
	}

	DriverParameters GetParameters() const {
		return parameters_;
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
	DriverParameters parameters_;
};

#endif
