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
