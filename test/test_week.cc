#include <week.h>

#include <orchis.h>
#include <sstream>

namespace {
    /* Wed 2018-10-17 CEST */
    const std::time_t wed = 1539810410;
    /* Wed 2019-07-03 CEST */
    const std::time_t july = 1562104800;

    void assert_fmt(const Week& w, const char* ref)
    {
	std::ostringstream oss;
	oss << w;
	orchis::assert_eq(oss.str(), ref);
    }
}

namespace week {

    void simple(orchis::TC)
    {
	Week w{wed};
	orchis::assert_eq(w.monday(), "2018-10-15");
	orchis::assert_eq(w.sunday(), "2018-10-21");

	w = Week{july};
	orchis::assert_eq(w.monday(), "2019-07-01");
	orchis::assert_eq(w.sunday(), "2019-07-07");
    }

    void by_timestamp(orchis::TC)
    {
	auto assert_week = [] (const char* ts) {
			       const Week ref{wed};
			       const Week w{ts};
			       orchis::assert_eq(w, ref);
			   };
	assert_week("2018-10-17T00:00:00");
	assert_week("2018-10-17T12:00:00");
	assert_week("2018-10-17T23:59:59");

	assert_week("2018-10-15T00:00:00");
	assert_week("2018-10-21T23:59:59");
    }

    void summer(orchis::TC)
    {
	auto assert_july = [] (const char* ts) {
			       const Week ref{july};
			       const Week w{ts};
			       orchis::assert_eq(w, ref);
			   };
	assert_july("2019-07-01T00:00:00");
	assert_july("2019-07-07T23:59:59");
    }

    void equality(orchis::TC)
    {
	const Week a{wed};
	const Week b{wed + 1};
	const Week c = a.prev();
	orchis::assert_eq(a, b);
	orchis::assert_neq(a, c);
    }

    void scale(orchis::TC)
    {
	const Week w{wed};
	auto assert_about = [&w] (double a, const char* ts) {
			     double t = w.scale(ts);
			     orchis::assert_le(a - .01, t);
			     orchis::assert_lt(t, a + .01);
			 };

	assert_about(0, "2018-10-15T00:00:00");
	assert_about(1, "2018-10-21T23:59:59");

	assert_about(1.0/7, "2018-10-16T00:00:00");
	assert_about(2.0/7, "2018-10-17T00:00:00");
	assert_about(3.0/7, "2018-10-18T00:00:00");
	assert_about(4.0/7, "2018-10-19T00:00:00");
	assert_about(5.0/7, "2018-10-20T00:00:00");
	assert_about(6.0/7, "2018-10-21T00:00:00");

	assert_about(6.5/7, "2018-10-21T12:00:00");

	assert_about(-1, "2018-10-08T00:00:00");
	assert_about( 2, "2018-10-29T00:00:00");
    }

    void prev(orchis::TC)
    {
	const Week w = Week{wed}.prev();
	orchis::assert_eq(w.monday(), "2018-10-08");
	orchis::assert_eq(w.sunday(), "2018-10-14");
    }

    void format(orchis::TC)
    {
	assert_fmt(Week{wed},          "15-21.10.2018");

	assert_fmt(Week{"2018-04-09"}, "9-15.4.2018");
	assert_fmt(Week{"2018-04-30"}, "30.4-6.5.2018");
	assert_fmt(Week{"2019-01-01"}, "31.12.2018-6.1.2019");
    }
}
