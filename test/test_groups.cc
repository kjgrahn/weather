#include <groups.h>

#include <string>
#include <list>
#include <vector>

#include <orchis.h>

namespace groups {

    bool backwards(char a, char b) { return b < a; }

    void empty(orchis::TC)
    {
	const char* a = "";
	const char* const b = a;

	auto c = pop_group(a, b, backwards);
	orchis::assert_eq(c, a);
	orchis::assert_eq(a, b);
    }

    void single(orchis::TC)
    {
	const char* a = "foo";
	const char* const b = a + 3;

	auto c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "foo");
	orchis::assert_eq(std::string{a, b}, "");
    }

    void two(orchis::TC)
    {
	const char* a = "foobcd";
	const char* const b = a + 6;

	auto c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "foo");
	orchis::assert_eq(std::string{a, b}, "bcd");

	c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "bcd");
	orchis::assert_eq(std::string{a, b}, "");
    }

    void ungrouped(orchis::TC)
    {
	const char* a = "fedcba";
	const char* const b = a + 6;

	const char* c;
	c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "f");
	c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "e");
	c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "d");
	c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "c");
	c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "b");
	c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "a");

	orchis::assert_eq(std::string{a, b}, "");
    }

    template <class C>
    void typed()
    {
	const char* s = "foobcd";
	const C container(s, s+6);

	auto a = begin(container);
	auto b = end(container);

	auto c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "foo");
	orchis::assert_eq(std::string{a, b}, "bcd");

	c = pop_group(a, b, backwards);
	orchis::assert_eq(std::string{c, a}, "bcd");
	orchis::assert_eq(std::string{a, b}, "");
    }

    void string(orchis::TC) { typed<std::string>(); }
    void list(orchis::TC)   { typed<std::list<char>>(); }
    void vector(orchis::TC) { typed<std::vector<char>>(); }
}
