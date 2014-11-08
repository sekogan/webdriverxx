#ifndef WEBDRIVERXX_TO_STRING_H
#define WEBDRIVERXX_TO_STRING_H

#include "meta_tools.h"
#include <string>
#include <sstream>

namespace webdriverxx {
namespace detail {

template<typename T>
void ToStream(const T& value, std::ostream& stream);

namespace to_string_impl {

template<typename T>
void WriteNonStreamableValue(const T&, std::ostream& stream) {
	stream << "<non-printable>";
}

} // namespace to_string_impl
} // detail
} // webdriverxx

namespace webdriverxx_to_string_impl {

template<typename T>
std::ostream& operator << (std::ostream& stream, const T& value) {
	webdriverxx::detail::to_string_impl::WriteNonStreamableValue(value, stream);
	return stream;
}

} // namespace webdriverxx_to_string_impl

namespace webdriverxx {
namespace detail {
namespace to_string_impl {

struct DefaultTag {};
struct IterableTag {};
struct StringTag {};

template<typename T>
struct Tag :
	if_<is_string<T>, type_is<StringTag>,
	if_<is_iterable<T>, type_is<IterableTag>,
	type_is<DefaultTag>
	>> {};

template<typename T>
void ToStreamImpl(const T& value, std::ostream& stream, DefaultTag) {
	using namespace webdriverxx_to_string_impl;
	stream << value;
}

template<typename T>
void ToStreamImpl(T* value, std::ostream& stream, DefaultTag) {
	stream << '*';
	ToStream(*value, stream);
}

inline
void ToStreamImpl(const char value, std::ostream& stream, DefaultTag) {
	stream << "'" << value << "'";
}

inline
void ToStreamImpl(const char* value, std::ostream& stream, StringTag) {
	stream << '"' << value << '"';
}

inline
void ToStreamImpl(const std::string& value, std::ostream& stream, StringTag) {
	ToStream(value.c_str(), stream);
}

template<typename T>
void ToStreamImpl(const T& value, std::ostream& stream, IterableTag) {
	auto it = std::begin(value);
	const auto end = std::end(value);
	int limit = 20;
	stream << "[";
	if (it != end) {
		ToStream(*it, stream);
		while (++it != end && --limit > 0) {
			stream << ", ";
			ToStream(*it, stream);
		}
	}
	stream << "]";
}

} // namespace to_string_impl

template<typename T>
void PrintTo(const T& value, std::ostream* stream) {
	using to_string_impl::ToStreamImpl;
	using to_string_impl::Tag;
	ToStreamImpl(value, *stream, typename Tag<T>::type());
}

template<typename T>
void ToStream(const T& value, std::ostream& stream)
{
	PrintTo(value, &stream); // for compatibility with Google Test's values printers
}

template<typename T>
std::string ToString(const T& value) {
	std::ostringstream s;
	ToStream(value, s);
	return s.str();
}

} // namespace detail
} // namespace webdriverxx

#endif
