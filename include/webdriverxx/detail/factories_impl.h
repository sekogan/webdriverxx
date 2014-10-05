#ifndef WEBDRIVERXX_DETAIL_FACTORIES_IMPL_H
#define WEBDRIVERXX_DETAIL_FACTORIES_IMPL_H

#include "factories.h"
#include "resource.h"
#include "finder.h"
#include "shared.h"
#include "../element.h"

namespace webdriverxx {
namespace detail {

class SessionFactory // noncopyable
	: public IElementFactory
	, public IFinderFactory
	, public SharedObjectBase
{
public:
	explicit SessionFactory(const Shared<Resource>& session_resource)
		: session_resource_(session_resource)
	{}

	virtual Element MakeElement(const std::string& id) {
		return Element(
			id,
			detail::MakeSubResource(session_resource_, "element", id),
			Shared<IFinderFactory>(this)
			);
	}

	virtual Finder MakeFinder(const Shared<Resource>& context) {
		return Finder(context, Shared<IElementFactory>(this));
	}

private:
	const Shared<Resource> session_resource_;
};

} // namespace detail
} // namespace webdriverxx

#endif
