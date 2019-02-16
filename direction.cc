/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "direction.h"

#include "spike.h"

#include <algorithm>


namespace {

    xml::attr attr(const char* name, const char* val) { return {name, val}; }
    xml::attr attr(const char* name, int val) { return {name, std::to_string(val)}; }

    bool windless(const Curves::Curve& curve)
    {
	auto windy = [] (const Curves::Sample& sample) {
			 return !!sample.wind_force;
		     };
	auto it = std::find_if(begin(curve), end(curve), windy);
	return it==end(curve);
    }

    bool same_dir(const Curves::Sample& a,
		  const Curves::Sample& b)
    {
	return a.wind_direction == b.wind_direction;
    }

    /**
     * The duration of an interval of Samples [a, b),
     * in hours.
     */
    template <class It>
    double duration(It a, It b)
    {
	const unsigned hours_per_week = 7 * 24;

	if(a==b) return 0;
	b--;
	auto dt = b->t - a->t;
	return hours_per_week * dt;
    }

    /**
     * The middle of a non-empty time interval [a, b).
     */
    template <class It>
    double middle(It a, It b)
    {
	b--;
	return (a->t + b->t)/2;
    }

    std::string direction_name(const Curves::Sample& sample)
    {
	switch(unsigned(sample.wind_direction.value())) {
	case   0: return "N";
	case  45: return "NO";
	case  90: return "O";
	case 135: return "SO";
	case 180: return "S";
	case 225: return "SV";
	case 270: return "V";
	case 315: return "NV";
	default: return "";
	}
    }
}


/**
 * Render the wind direction, if any, in 'curve', into 'area'.
 *
 * It's rendered as text, centered over longer time periods with the
 * same wind.  This is partly because the source data only knows about
 * eight discrete wind directions.
 *
 * Returns true if anything was rendered.
 */
bool direction(xml::ostream& xos, const Area& area,
	       const Curves::Curve& curve)
{
    if(windless(curve)) return false;

    xos << xml::elem("text")
	<< attr("font-family", "serif")
	<< attr("font-weight", "bold")
	<< attr("font-style", "italic")
	<< attr("fill", "#4060c0")
	<< attr("opacity", ".9")
	<< attr("font-size", "12")
	<< attr("text-anchor", "middle")
	<< attr("y", area.offset + 12 + 6);

    auto a = begin(curve);
    const auto b = end(curve);
    while(a!=b) {
	auto c = pop_group2(a, b, same_dir);
	if(duration(c, a) < 6) continue;

	xos << xml::elem("tspan")
	    << attr("x", area.xscale(middle(c, a)))
	    << direction_name(*c)
	    << xml::end;
    }

    xos << xml::end;
    return true;
}
