#ifndef WEBDRIVERXX_CAPABILITIES_H
#define WEBDRIVERXX_CAPABILITIES_H

#include <string>
#include <utility>
#include <picojson.h>

namespace webdriverxx {

class Capabilities // copyable
{
public:
	Capabilities()
	{}

	explicit Capabilities(const picojson::object& object)
		: object_(object)
	{}

	template<typename T>
	Capabilities& operator << (const std::pair<std::string,T>& name_value)
	{
		object_[name_value.first] = picojson::value(name_value.second);
	}

	bool Contains(const std::string& name) const
	{
		return object_.find(name) != object_.end();
	}

	bool GetBool(const std::string& name) const
	{
		picojson::object::const_iterator it = object_.find(name);
		return it == object_.end() ? false : it->second.evaluate_as_boolean();
	}

	std::string GetString(const std::string& name) const
	{
		picojson::object::const_iterator it = object_.find(name);
		return it == object_.end() ? std::string() : it->second.to_str();
	}

	const picojson::object& Get() const
	{
		return object_;
	}

private:
	picojson::object object_;
};

} // namespace webdriverxx

#endif
