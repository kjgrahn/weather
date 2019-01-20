/* -*- c++ -*-
 *
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
#ifndef WEATHER_SPIKE_H
#define WEATHER_SPIKE_H

#include <algorithm>

/**
 * This algorithm sees a range as split into sub-ranges, groups, by
 * equivalence, except spikes of "non-equivalence" are tolerated.
 * For example,
 *
 * aaaaaa
 * abaaaa
 * a bb aaa
 * ababaa
 *
 * After c = pop_group2(a, b), the first group is [c, a), and the
 * remainder is [a, b).  Repeat until empty.
 *
 * I use it to ignore spikes in the wind direction series.
 */
template <class It, class Equivalence>
It pop_group2(It& first, const It last, Equivalence eq)
{
    using T = decltype(*first);
    auto neq = [eq] (T a, T b) { return !eq(a, b); };

    const It a = first;
    while(first != last) {
	It it = std::adjacent_find(first, last, neq);
	if(it!=last) {
	    It it2 = next(it, 2);
	    if(it2!=last && eq(*it, *it2)) {
		first = it2;
	    }
	    else {
		first = next(it);
		break;
	    }
	}
	else {
	    first = last;
	}
    }
    return a;
}

#endif
