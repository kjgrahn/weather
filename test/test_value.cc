#include <value.h>

#include <orchis.h>

namespace {

    std::string str(Value val)
    {
	std::ostringstream oss;
	oss << val;
	return oss.str();
    }
}

namespace value {

    using orchis::TC;
    void assert_eq(Value a, Value b) { orchis::assert_eq(a, b); }
    void assert_neq(Value a, Value b) { orchis::assert_neq(a, b); }
    void assert_lt(Value a, Value b) { orchis::assert_lt(a, b); }

    namespace compare {

	void eq(TC)
	{
	    assert_eq({0.0}, {0.0});
	    assert_eq({42.1}, {42.1});
	    assert_eq({-42.1}, {-42.1});
	}

	void neq(TC)
	{
	    assert_neq({-42.1}, {-42.2});
	}

	void lt(TC)
	{
	    assert_lt({0.0}, {0.1});
	    assert_lt({-5.0}, {-4.9});
	}
    }

    namespace construct {

	void from_double(TC)
	{
	    orchis::assert_eq(str(Value{0.0}), "0.0");

	    orchis::assert_eq(str(Value{100.0}),  "100.0");
	    orchis::assert_eq(str(Value{100.49}), "100.5");
	    orchis::assert_eq(str(Value{100.51}), "100.5");

	    orchis::assert_eq(str(Value{-100.0}),  "-100.0");
	    orchis::assert_eq(str(Value{-100.49}), "-100.5");
	    orchis::assert_eq(str(Value{-100.51}), "-100.5");

	    assert_eq({-100.51}, {-100.49});
	}

	void from_buffer(TC)
	{
	    constexpr char p[] = "-59.99<garbage>";
	    assert_eq({p, p+2}, {-5});
	    assert_eq({p, p+3}, {-59});
	    assert_eq({p, p+4}, {-59});
	    assert_eq({p, p+5}, {-59.9});
	    assert_eq({p, p+6}, {-60.0});
	}
    }
}
