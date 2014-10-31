#ifndef WEBDRIVERXX_DETAIL_META_TOOLS_H
#define WEBDRIVERXX_DETAIL_META_TOOLS_H

#include <type_traits>
#include <utility>
#include <iterator>
#include <string>

namespace webdriverxx {
namespace detail {

template<class T>
struct type_is { typedef T type; };

template<class>
struct to_void : type_is<void> {};

template<class T>
T& value_ref(); // MSVC2010 doesn't have std::declval

template<class T>
class is_iterable {
	template<class U>
	static std::false_type test(U, ...);
	template<class U>
	static std::true_type test(U& value, decltype(std::distance(std::begin(value),std::end(value))));

	typedef decltype(test(value_ref<T>(), 0)) verdict;

public:
	static const bool value = verdict::value;
};

template<class T>
struct is_string : std::is_convertible<const T&, std::string> {};

template<class C, class T1, class T2>
struct if_ : std::conditional<C::value, typename T1::type, typename T2::type> {};

} // namespace detail
} // namespace webdriverxx

#endif
