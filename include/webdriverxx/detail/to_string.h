#ifndef WEBDRIVERXX_TO_STRING_H
#define WEBDRIVERXX_TO_STRING_H

#include <string>
#include <sstream>
#include <type_traits>

namespace webdriverxx {
namespace detail {

template<typename T>
inline
std::string ToString(const T& value);

template<typename S, typename T>
inline
void WriteNonStreamableValue(S& s, const T& /*value*/) {
	s << "<non-printable>";
}

}}

namespace webdriverxx_to_string_streamable_filter {

template<typename T>
inline
std::ostream& operator << (std::ostream& stream, const T& value) {
	::webdriverxx::detail::WriteNonStreamableValue(stream, value);
	return stream;
}

} // namespace webdriverxx_to_string_streamable_filter

namespace webdriverxx {
namespace detail {

struct ToStringStreamableFilter {
	template<typename T>
	static std::string Apply(const T& value) {
		std::ostringstream s;
		using namespace webdriverxx_to_string_streamable_filter;
		s << value;
		return s.str();
	}
};

template<typename T>
inline
T& Declval(); // MSVC2010 does not support std::declval

template<typename NextFilter>
struct ToStringContainerFilter {
	template<typename T>
	static std::string Apply(const T& value) {
		return Impl(value, 0);
	}

private:
	template<typename T>
	static std::string Impl(const T& value, decltype(&*std::begin(Declval<T>()))) {
		auto it = std::begin(value);
		const auto end = std::end(value);
		int limit = 20;
		std::ostringstream s;
		s << "[";
		if (it != end) {
			s << ToString(*it);
			while (++it != end && --limit > 0)
				s << ", " << ToString(*it);
		}
		s << "]";
		return s.str();
	}

	template<typename T>
	static std::string Impl(const T& value, ...) {
		return NextFilter::Apply(value);
	}
};

template<typename NextFilter>
struct ToStringSpecializationFilter {
	template<typename T>
	static std::string Apply(const T& value) {
		return Impl<T>::Apply(value);
	}

private:
	template<typename T>
	struct Impl {
		static std::string Apply(const T& value) {
			return NextFilter::Apply(value);
		}
	};

	template<typename T>
	struct Impl<T*> {
		static std::string Apply(T* value) {
			return std::string("*") + ToString(*value);
		}
	};
};

template<typename T>
inline
void PrintTo(const T& value, ::std::ostream* stream) {
	namespace detail = ::webdriverxx::detail;
	*stream <<
		detail::ToStringContainerFilter<
		detail::ToStringSpecializationFilter<
		detail::ToStringStreamableFilter
			>>::Apply(value);
}

inline
void PrintTo(char value, ::std::ostream* stream) {
	*stream << "'" << value << "'";
}

inline
void PrintTo(const char* value, ::std::ostream* stream) {
	*stream << '"' << value << '"';
}

inline
void PrintTo(char* value, ::std::ostream* stream) {
	PrintTo(static_cast<const char*>(value), stream);
}

inline
void PrintTo(const std::string& value, ::std::ostream* stream) {
	PrintTo(value.c_str(), stream);
}

template<typename T>
inline
std::string ToString(const T& value) {
	std::ostringstream s;
	PrintTo(value, &s);
	return s.str();
}

} // namespace detail
} // namespace webdriverxx

#endif
