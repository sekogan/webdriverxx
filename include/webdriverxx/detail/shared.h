#ifndef WEBDRIVERXX_DETAIL_SHARED_H
#define WEBDRIVERXX_DETAIL_SHARED_H

#include <algorithm>
#include <new>

namespace webdriverxx {
namespace detail {

// Copyable, not thread safe
template<typename T>
class Shared {
public:
	explicit Shared(T* ptr = 0)
		: ptr_(ptr)
		, ref_(ptr ? new (std::nothrow) unsigned(1) : 0)
	{
		if (ptr_ && !ref_) {
			delete ptr_;
			throw std::bad_alloc();
		}
	}

	Shared(const Shared& other)
		: ptr_(other.ptr_)
		, ref_(other.ref_)
	{
		if (ref_) ++(*ref_);
	}

	template<typename T2>
	Shared(const Shared<T2>& other) 
		: ptr_(other.ptr_)
		, ref_(other.ref_)
	{
		if (ref_) ++(*ref_);
	}

	~Shared() {
		if (ref_ && --(*ref_) == 0) {
			delete ptr_;
			delete ref_;
		}
	}

	Shared& operator = (const Shared& other) {
		if (&other != this)
			Shared<T>(other).Swap(*this);
		return *this;
	}

	void Swap(Shared& other) {
		std::swap(ptr_, other.ptr_);
		std::swap(ref_, other.ref_);
	}

	T& operator * () const {
		return *ptr_;
	}

	T* operator -> () const {
		return ptr_;
	}

	T* Get() const {
		return ptr_;
	}

	operator T* () const {
		return ptr_;
	}

private:
	template<typename T2>
	friend class Shared;

	T* ptr_;
	unsigned* ref_;
};

} // namespace detail
} // namespace webdriverxx

#endif
