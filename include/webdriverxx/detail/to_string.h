#ifndef WEBDRIVERXX_TO_STRING_H
#define WEBDRIVERXX_TO_STRING_H

#include <string>
#include <sstream>
#include <type_traits>

namespace webdriverxx {
namespace detail {

template<typename T>
std::string ToString(const T& value);

std::string ToString(char value) {
	return std::string("'") + value + "'";
}

template<typename T>
T& Declval();

struct ToStringPrintableFilter {
	template<typename T>
	static std::string Apply(const T& value) {
		return Impl(value, 0);
	}

private:
	template<typename T>
	static std::string Impl(const T& value, decltype(Declval<std::ostream>() << Declval<T>())*) {
		std::ostringstream s;
		s << value;
		return s.str();
	}

	template<typename T>
	static std::string Impl(const T& /*value*/, ...) {
		return "<non-printable>";
	}
};

template<typename NextFilter>
struct ToStringContainerFilter {
	template<typename T>
	static std::string Apply(const T& value) {
		return Impl(value, 0);
	}

private:
	template<typename T>
	static std::string Impl(const T& value, decltype(std::begin(Declval<T>()))*) {
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
struct ToStringStringFilter {
	template<typename T>
	static std::string Apply(const T& value) {
		return Impl(value, 0);
	}

private:
	template<typename T>
	static std::string Impl(const T& value, typename std::enable_if<std::is_convertible<T,std::string>::value>::type*) {
		return std::string(1, '"') + std::string(value) + '"';
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
std::string ToString(const T& value) {
	namespace detail = ::webdriverxx::detail;
	return detail::ToStringStringFilter<
		detail::ToStringSpecializationFilter<
		detail::ToStringContainerFilter<
		detail::ToStringPrintableFilter
		>>>::Apply(value);
}

} // namespace detail
} // namespace webdriverxx

#endif
