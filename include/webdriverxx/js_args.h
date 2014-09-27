#ifndef WEBDRIVERXX_JS_ARGS_H
#define WEBDRIVERXX_JS_ARGS_H 

#include "detail/conversions.h"
#include <picojson.h>

namespace webdriverxx {

class WebDriver;

class JsArgs // copyable
{
public:
	JsArgs()
		: args_(picojson::array()) {}

	template<typename T>
	JsArgs& operator << (const T& arg) {
		args_.get<picojson::array>().push_back(detail::ToJson(arg));
		return *this;
	}

private:
	friend class WebDriver;
	picojson::value args_;
};

} // namespace webdriverxx 

#endif
