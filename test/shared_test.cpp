#include <webdriverxx/detail/shared.h>
#include <gtest/gtest.h>
#include <cassert>

namespace test {

using namespace webdriverxx::detail;

struct WidgetMonitor {
	WidgetMonitor()
		: created(0)
		, copied(0)
		, deleted(0)
	{}

	int created;
	int copied;
	int deleted; 
};

struct Simple : SharedObjectBase {
	int n;
	Simple() : n() {}
};

class Widget : public SharedObjectBase {
public:
	Widget(WidgetMonitor& monitor)
		: monitor(monitor) {
		++monitor.created;
	}

	Widget(const Widget& other)
		: monitor(other.monitor) {
		++monitor.created;
	}
	
	Widget& operator = (const Widget& other) {
		assert(&monitor == &other.monitor);
		++monitor.copied;
		return *this;
	}

	virtual ~Widget() {
		++monitor.deleted;		
	}

private:
	WidgetMonitor& monitor;
};

class WidgetSubclass : public Widget {
public:
	WidgetSubclass(WidgetMonitor& monitor) : Widget(monitor) {}
};

TEST(WidgetMonitor, ZeroByDefault) {
	WidgetMonitor m;
	ASSERT_EQ(0, m.created);
	ASSERT_EQ(0, m.copied);
	ASSERT_EQ(0, m.deleted);
}

TEST(WidgetMonitor, ShowsCreateAndDelete) {
	WidgetMonitor m;
	((void)Widget(m));
	ASSERT_EQ(1, m.created);
	ASSERT_EQ(0, m.copied);
	ASSERT_EQ(1, m.deleted);
}

TEST(WidgetMonitor, ShowsCopy) {
	WidgetMonitor m;
	Widget w1(m);
	Widget w2(m);
	w2 = w1;
	ASSERT_EQ(1, m.copied);

}

TEST(WidgetMonitor, MonitorsWidgetSubclass) {
	WidgetMonitor m;
	((void)WidgetSubclass(m));
	ASSERT_EQ(1, m.created);
	ASSERT_EQ(0, m.copied);
	ASSERT_EQ(1, m.deleted);
}

TEST(Shared, DeletesWidget) {
	WidgetMonitor m;
	Shared<Widget>(new Widget(m));
	ASSERT_EQ(1, m.created);
	ASSERT_EQ(1, m.deleted);
}

TEST(Shared, ShareSingleWidget) {
	WidgetMonitor m;
	{
		Shared<Widget> a(new Widget(m));
		Shared<Widget> b = a;
		Shared<Widget> c;
		c = b;
	}
	ASSERT_EQ(1, m.created);
	ASSERT_EQ(0, m.copied);
	ASSERT_EQ(1, m.deleted);
}

TEST(Shared, SupportsImplicitTypecasts) {
	WidgetMonitor m;
	{
		Shared<Widget> c;
		Shared<WidgetSubclass> a(new WidgetSubclass(m));
		Shared<Widget> b = a;
		c = b;
	}
	ASSERT_EQ(1, m.created);
	ASSERT_EQ(0, m.copied);
	ASSERT_EQ(1, m.deleted);
}

TEST(Shared, CanBeUsedInBoolContext) {
	Shared<Simple> s1;
	Shared<Simple> s2(new Simple);
	ASSERT_TRUE(!s1);
	ASSERT_TRUE(!!s2);
}

TEST(Shared, CanBeDereferenced) {
	Simple* p = new Simple;
	Shared<Simple> s(p);
	p->n = 123;
	ASSERT_EQ(123, (*s).n);
	ASSERT_EQ(123, s->n);
}

TEST(Shared, CanBeCompared) {
	Shared<Simple> s1;
	Shared<Simple> s2(new Simple);
	Shared<Simple> s3 = s2;
	Shared<Simple> s4(new Simple);
	ASSERT_TRUE(s1 != s2);
	ASSERT_TRUE(s1 != s3);
	ASSERT_TRUE(s2 == s3);
	ASSERT_TRUE(s2 != s4);
	ASSERT_TRUE(s1 == s1);
	ASSERT_TRUE(s2 == s2);
	ASSERT_TRUE(s3 == s3);
	ASSERT_TRUE(s4 == s4);
	ASSERT_TRUE(s1 == 0);
	ASSERT_TRUE(s2 != 0);
}

} // namespace test
