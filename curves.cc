/*
 * Copyright (c) 2018 Jörgen Grahn
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
#include "curves.h"

#include "files...h"
#include "week.h"

#include <iostream>
#include <cctype>
#include <cstdlib>
#include <algorithm>


namespace {

    inline bool isspace(char ch)
    {
	return std::isspace(static_cast<unsigned char>(ch));
    }

    /**
     * Trim whitespace to the left in [a, b).
     */
    inline
    const char* ws(const char* a, const char* b)
    {
	while(a!=b && isspace(*a)) a++;
	return a;
    }

    /**
     * Trim non-whitespace to the left in [a, b).
     */
    inline
    const char* non_ws(const char* a, const char* b)
    {
	while(a!=b && !isspace(*a)) a++;
	return a;
    }

    /**
     * Trim whitespace to the right in [a, b), so that
     * it's either empty or ends with non-whitespace.
     */
    inline
    const char* trimr(const char* a, const char* b)
    {
	while(a!=b && isspace(*(b-1))) b--;
	return b;
    }
}


Curves::Curves(const Week& week, Files& files, std::ostream& err)
{
    Sample* cur = nullptr;

    auto add = [&cur, &week, this] (const char* a, const char* b) -> Sample* {
		   const std::string ts{a, b};
		   double t = week.scale(ts);
		   bool in_range = 0 <= t && t <= 1;
		   if(!in_range) return nullptr;

		   if(!cur) {
		       val.push_back({});
		   }
		   auto& v = val.back();
		   v.emplace_back(t);
		   return &v.back();
	       };

    std::string s;
    while(files.getline(s)) {
	/* "   foo   :   bar  "
	 *     a  d  c   e  b
	 */
	const char* a = s.data();
	const char* b = a + s.size();
	a = ws(a, b);
	b = trimr(a, b);
	if(a==b || *a=='#') continue;
	const char* const c = std::find(a, b, ':');
	if(c==b) {
	    err << files.position() << ": malformed line \"" << s << "\"\n";
	    continue;
	}
	const char* const d = trimr(a, c);
	const char* const e = ws(c+1, b);
	if(a==d || e==b) {
	    err << files.position() << ": malformed line \"" << s << "\"\n";
	    continue;
	}

	const std::string key{a, d};
	if(key=="date") {
	    cur = add(e, b);
	}
	else if(!cur) {
	    continue;
	}
	else if(key=="temperature.air") {
	    cur->temperature_air = {e, b};
	}
	else if(key=="wind.force") {
	    cur->wind_force = {e, b};
	}
	else if(key=="wind.force.max") {
	    cur->wind_force_max = {e, b};
	}
    }

    for(auto& curve : val) {
	auto e = std::remove_if(std::begin(curve), std::end(curve),
				[] (Sample s) { return s.empty(); });
	curve.erase(e, std::end(curve));
    }
}
