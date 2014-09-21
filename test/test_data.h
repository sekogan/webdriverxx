#ifndef WEBDRIVERXX_TEST_DATA_H
#define WEBDRIVERXX_TEST_DATA_H

#include <webdriverxx/capabilities.h>
#include <string>

const char* const kPhantomUrl = "http://localhost:7777/";

struct DriverParameters
{
	std::string url;
	webdriverxx::Capabilities required;
	webdriverxx::Capabilities desired;
};

extern DriverParameters g_driver;

#endif
