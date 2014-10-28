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

template<class T, class = void>
struct is_iterable : std::false_type {};

template<class T>
struct is_iterable<T, typename to_void<decltype(
	std::begin(std::declval<const T&>()), std::end(std::declval<const T&>())
	)>::type> : std::true_type {};

template<class T>
struct is_string : std::is_convertible<const T&, std::string> {};

template<class C, class T1, class T2>
struct if_ : std::conditional<C::value, typename T1::type, typename T2::type> {};

} // namespace detail
} // namespace webdriverxx

#endif
