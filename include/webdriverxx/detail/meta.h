#ifndef WEBDRIVERXX_META_H
#define WEBDRIVERXX_META_H

namespace webdriverxx {
namespace detail {

template<typename T>
inline
T& ValueRef(); // MSVC2010 does not support std::declval

} // namespace detail
} // namespace webdriverxx

#endif
