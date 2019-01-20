#include <spike.h>

#include <string>
#include <orchis.h>

namespace groups2 {

    bool eq(char a, char b) { return a==b; }

    /**
     * Assert that s + t groups into two groups, s and t.
     */
    void assert_groups(const std::string& s, const std::string& t)
    {
	const auto src = s + t;
	auto a = begin(src);
	const auto b = end(src);

	auto c = pop_group2(a, b, eq);
	orchis::assert_eq(std::string(c, a), s);

	c = pop_group2(a, b, eq);
	orchis::assert_eq(std::string(c, a), t);

	orchis::assert_eq(std::string(a, b), "");
    }

    void assert_groups(const std::string& s)
    {
	const auto src = s;
	auto a = begin(src);
	const auto b = end(src);

	auto c = pop_group2(a, b, eq);
	orchis::assert_eq(std::string(c, a), s);

	orchis::assert_eq(std::string(a, b), "");
    }

    void empty(orchis::TC)
    {
	assert_groups("");
    }

    void no_spike(orchis::TC)
    {
	assert_groups("aaaa", "bbbb");
	assert_groups("a", "bbbb");
	assert_groups("aaaa", "b");
    }

    void spike1(orchis::TC)
    {
	assert_groups("aba");
	assert_groups("abaa", "bbbb");
    }

    void spike2(orchis::TC)
    {
	assert_groups("ababa");
	assert_groups("ababa", "bbbb");
    }
}
