#ifndef WEBDRIVERXX_JS_ARGS_H
#define WEBDRIVERXX_JS_ARGS_H 

#include "conversions.h"
#include <picojson.h>

namespace webdriverxx {

class Session;

class JsArgs // copyable
{
public:
	JsArgs()
		: args_(picojson::array()) {}

	// This member works out of the box for scalar values, Elements and std::vector
	// with scalar values or Elements. Additional code is needed to use it
	// with custom objects and custom containers. Take a look
	// to the TestJsExecutor/CanPassArray, TestJsExecutor/CanPassCustomArray
	// and TestJsExecutor/CanPassCustomObjects tests to get details.
	template<typename T>
	JsArgs& operator << (const T& arg) {
		args_.get<picojson::array>().push_back(ToJson(arg));
		return *this;
	}

	// Alternative backdoor for passing custom data structures as arguments.
	JsArgs& operator << (const picojson::value& arg) {
		args_.get<picojson::array>().push_back(arg);
		return *this;
	}

private:
	friend class Session;
	picojson::value args_;
};

} // namespace webdriverxx 

#endif
