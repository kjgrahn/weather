/* Copyright (c) 2016 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "sample.h"

#include <iostream>
#include <algorithm>


namespace {

    std::ostream& ljust(std::ostream& os, const std::string& s, size_t n)
    {
	os << s;
	if(s.size() < n) {
	    os << std::string(n - s.size(), ' ');
	}
	return os;
    }

    std::ostream& rjust(std::ostream& os, const std::string& s, size_t n)
    {
	if(s.size() < n) {
	    os << std::string(n - s.size(), ' ');
	}
	return os << s;
    }
}

std::ostream& operator<< (std::ostream& os, const Sample& val)
{
    os << "date: " << val.time << '\n';

    auto put = [&val] (std::ostream& os, const char* name) {
	const auto it = val.data.find(name);
	if(it==end(val.data)) return;

	ljust(os, it->first, 16) << ": ";
	rjust(os, it->second, 5) << '\n';
    };

    put(os, "temperature.road");
    put(os, "temperature.air");
    put(os, "humidity");
    put(os, "wind.direction");
    put(os, "wind.force");
    put(os, "wind.force.max");

    return os;
}

/**
 * Sort and print a sequence of samples to 'os', but first print
 * 'prefix'.
 */
void render(std::ostream& os, const char* prefix,
	    Samples samples)
{
    std::sort(begin(samples), end(samples));

    os << prefix;
    const char* delimiter = "";
    for(const auto& sample: samples) {
	os << delimiter << sample;
	delimiter = "\n";
    }
}
