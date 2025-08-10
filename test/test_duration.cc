#include <duration.h>

#include <orchis.h>
#include <sstream>

namespace duration {

    using orchis::TC;

    void assert_valid(const Duration& d, const char* s, unsigned)
    {
	orchis::assert_true(d.valid());
	std::ostringstream ss;
	ss << d;
	orchis::assert_eq(ss.str(), s);
    }

    void assert_invalid(const Duration& d)
    {
	orchis::assert_false(d.valid());
    }

    void simple_hours(TC)
    {
	assert_valid(Duration{"1"},     "-0.01:00", 6);
	assert_valid(Duration{"1h"},    "-0.01:00", 6);
	assert_valid(Duration{"8h"},    "-0.08:00", 6*8);
	assert_valid(Duration{"0d8h"},  "-0.08:00", 6*8);
	assert_valid(Duration{"25h"},   "-1.01:00", 6*25);
    }

    void simple_days(TC)
    {
	assert_valid(Duration{"1d1h"},  "-1.01:00", 6*25);
	assert_valid(Duration{"1d01h"}, "-1.01:00", 6*25);
	assert_valid(Duration{"1d25h"}, "-2.01:00", 6*49);

	assert_valid(Duration{"365d"},  "-365.00:00", 6*24*365);
    }

    void invalid(TC)
    {
	assert_invalid(Duration{""});
	assert_invalid(Duration{"0"});
	assert_invalid(Duration{"foo"});
	assert_invalid(Duration{"d10"});
	assert_invalid(Duration{"d10h"});
	assert_invalid(Duration{"h"});
	assert_invalid(Duration{"10x"});
	assert_invalid(Duration{"4dxh"});
	assert_invalid(Duration{"4h5d"});

	assert_invalid(Duration{"1000d"});
    }
}
