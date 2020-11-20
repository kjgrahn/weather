/*
 * Copyright (c) 2020 Jörgen Grahn
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
#include "duration.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

namespace {

    /**
     * Don't tolerate more than 999 days, or badness.
     */
    unsigned trim(unsigned n, bool good)
    {
	if (n > 999*24) return 0;
	if (!good) return 0;
	return n;
    }
}

Duration::Duration(const std::string& s)
    : h(0)
{
    const char* a = s.c_str();
    const auto e = a + s.size();

    const auto D = std::find(a, e, 'd');
    const auto H = std::find(a, e, 'h');

    if (D<H && H+1==e) {
	char* b;
	unsigned d = std::strtoul(a, &b, 10);
	const bool vd = a!=b && b==D;

	a = b+1;
	unsigned n = std::strtoul(a, &b, 10);

	h = trim(d * 24 + n,
		 vd && a!=b && b==H);
    }
    else if (D+1==e) {
	char* b;
	unsigned n = std::strtoul(a, &b, 10);
	h = trim(n * 24, a!=b && b==D);
    }
    else if (H+1==e) {
	char* b;
	unsigned n = std::strtoul(a, &b, 10);
	h = trim(n, a!=b && b==H);
    }
    else if (D==e && H==e) {
	char* b;
	unsigned n = std::strtoul(a, &b, 10);
	h = trim(n, a!=b && b==e);
    }
}

std::ostream& Duration::put(std::ostream& os) const
{
    char buf[11];
    std::snprintf(buf, sizeof buf, "-%u.%02u:00", h / 24, h % 24);
    return os << buf;
}
