#include <curves.h>
#include <week.h>
#include <files...h>

#include <orchis.h>
#include <sstream>


namespace {
    std::string sample(const char* date)
    {
	std::ostringstream ss;
	ss << "date: " << date << '\n'
	   << "temperature.road:   8.4\n"
	   << "temperature.air :   6.7\n"
	   << "humidity        :  92.9\n"
	   << "wind.direction  :   135\n"
	   << "wind.force      :   2.5\n"
	   << "wind.force.max  :   3.4\n";
	return ss.str();
    }
}

namespace curves {

    using orchis::TC;

    void nothing(TC)
    {
	std::stringstream ss;
	ss << sample("2018-10-06T22:20:00")
	   << sample("2018-12-06T22:20:00");
	Files f(ss);

	std::ostringstream err;
	const Curves curves{Week{"2018-11-23T02:03:00"}, f, err};

	orchis::assert_eq(err.str(), "");
	orchis::assert_true(curves.begin() == curves.end());
    }

    void single(TC)
    {
	std::stringstream ss;
	ss << "date: 2018-11-19T00:00:00\n"
	   << "temperature.road:   8.4\n"
	   << "temperature.air :   6.7\n"
	   << "humidity        :  92.9\n"
	   << "wind.direction  :   135\n"
	   << "wind.force      :   2.5\n"
	   << "wind.force.max  :   3.4\n";
	Files f(ss);

	std::ostringstream err;
	const Curves curves{Week{"2018-11-23T02:03:00"}, f, err};

	orchis::assert_eq(err.str(), "");
	auto it = curves.begin();
	orchis::assert_false(it == curves.end());
	const auto curve = *it++;
	orchis::assert_true(it == curves.end());

	orchis::assert_eq(curve.size(), 1);
	auto sample = curve.front();
	orchis::assert_eq(sample.t, 0);
	orchis::assert_eq(sample.temperature_air, 6.7);
	orchis::assert_eq(sample.wind_force,      2.5);
	orchis::assert_eq(sample.wind_force_max,  3.4);
    }

    namespace pattern {

	std::string count(const Curves& curves)
	{
	    std::string acc;
	    std::for_each(std::begin(curves), std::end(curves),
			  [&acc] (const Curves::Curve& val) {
			      acc += std::to_string(val.size());
			  });
	    return acc;
	}

	void assert_parses(const char* time, Files& src,
			   const char* counts)
	{
	    std::ostringstream err;
	    const Curves curves{Week{time}, src, err};
	    orchis::assert_eq(err.str(), "");
	    orchis::assert_eq(count(curves), counts);
	}

	void wW(TC)
	{
	    std::stringstream ss;
	    ss << sample("2018-10-06T22:20:00")
	       << sample("2018-11-24T10:13:00");
	    Files f(ss);
	    assert_parses("2018-11-23T02:03:00", f, "1");
	}

	void Ww(TC)
	{
	    std::stringstream ss;
	    ss << sample("2018-11-24T10:13:00")
	       << sample("2018-10-06T22:20:00");
	    Files f(ss);
	    assert_parses("2018-11-23T02:03:00", f, "1");
	}

	void WWW(TC)
	{
	    std::stringstream ss;
	    ss << sample("2018-11-24T10:00:00")
	       << sample("2018-11-23T10:00:00")
	       << sample("2018-11-22T10:00:00");
	    Files f(ss);
	    assert_parses("2018-11-23T02:03:00", f, "3");
	}

	void WwwWW(TC)
	{
	    std::stringstream ss;
	    ss << sample("2018-11-24T10:00:00")
	       << sample("2018-12-01T00:00:00")
	       << sample("2018-10-01T00:00:00")
	       << sample("2018-11-23T10:00:00")
	       << sample("2018-11-22T10:00:00");
	    Files f(ss);
	    assert_parses("2018-11-23T02:03:00", f, "12");
	}

	void WwWWwW(TC)
	{
	    std::stringstream ss;
	    ss << sample("2018-11-24T10:00:00")
	       << sample("2018-12-01T00:00:00")
	       << sample("2018-11-23T10:00:00")
	       << sample("2018-11-23T11:00:00")
	       << sample("2018-10-01T00:00:00")
	       << sample("2018-11-22T10:00:00");
	    Files f(ss);
	    assert_parses("2018-11-23T02:03:00", f, "121");
	}
    }

    namespace parse {
    }
}
