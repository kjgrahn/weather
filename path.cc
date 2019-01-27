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
#include "path.h"

#include "reckon.h"
#include "groups.h"

#include <sstream>


/**
 * Format the line as an SVG <path d=...> attribute. The line may
 * contain holes (if an hour of samples are missing, or if there's
 * backtracking).
 *
 *   M x y  l dx dy dx dy ...
 *   M x y  l dx dy dx dy ...
 *   ...
 */
std::string path::line(double hour, const std::vector<std::pair<double, double>>& v)
{
    auto begin = std::begin(v);
    const auto end = std::end(v);

    auto hole = [hour] (std::pair<double, double> a,
			std::pair<double, double> b) {
		    auto t0 = a.first;
		    auto t1 = b.first;
		    return t1 <= t0 || t0 + hour < t1;
		};
    std::ostringstream ss;

    while(begin != end) {
	auto a = pop_group(begin, end, hole);
	if(std::distance(a, begin) < 2) continue;

	reckon(ss, a, begin) << '\n';
    }
    std::string s = ss.str();
    if(s.size()) s.pop_back();
    return s;
}

namespace {

    using V = std::vector<std::pair<double, double>>;

    /**
     * Given a subsequence of 'v' with either all rain or no rain,
     * return something fill-able.
     *
     * - No rain: return an empty sequence.
     *
     * - Rain: extend with the samples before and after, so a point
     *   with no rain is present.  If v begins or ends with rain
     *   (t, r), do it by adding a sample (t, nil).
     */
    V shover(V::const_iterator a, V::const_iterator b,
	     const V& v,
	     const double nil)
    {
	V acc;
	if(a->second==nil) return acc;

	if(a==begin(v)) {
	    acc.push_back({a->first, nil});
	}
	else {
	    a--;
	}
	if(b==end(v)) {
	    acc.insert(end(acc), a, b);
	    b--;
	    acc.push_back({b->first, nil});
	}
	else {
	    b++;
	    acc.insert(end(acc), a, b);
	}
	return acc;
    }
}

/**
 * Like line(), but for SVG path filling.  These are polygons with the
 * vertex from the last to the first implied, so we need to start and
 * end at y=0 or it will look funny.
 *
 * This means a period of rain needs to ramp up from the previous
 * sample, and ramp down similarly.  Plus special handling if the data
 * begins or ends with rain.  For example:
 *
 *    t rain
 *    ------
 *    0  5.0  M 0 0  L 0 5
 *    1  2.0  L 1 2  L 1 0
 *    2  0.0
 *    3  4.0  M 2 0  L 3 4
 *    4  3.0  L 4 3  L 4 0
 *    end
 */
std::string path::fill(double nil, const std::vector<std::pair<double, double>>& v)
{
    auto begin = std::begin(v);
    const auto end = std::end(v);

    // cut at change to or from rain
    auto change = [nil] (std::pair<double, double> a,
			std::pair<double, double> b) {
		      bool adry = a.second == nil;
		      bool bdry = b.second == nil;
		      return adry ^ bdry;
		  };
    std::ostringstream ss;

    while(begin != end) {
	auto a = pop_group(begin, end, change);
	auto sh = shover(a, begin, v, nil);
	if(sh.empty()) continue;

	reckon(ss, std::begin(sh), std::end(sh)) << '\n';
    }
    std::string s = ss.str();
    if(s.size()) s.pop_back();
    return s;
}
