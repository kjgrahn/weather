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
#include "week.h"

#include <iostream>
#include <cstdio>
#include <time.h>

using std::time_t;

namespace {
    /**
     * Parse a time in ISO 2018-10-06T22:20:00 format,
     * local time implied.
     */
    time_t parse(const std::string& ts)
    {
	std::tm tm = {};
	tm.tm_isdst = -1;
	strptime(ts.c_str(), "%FT%H:%M:%S", &tm);
	return mktime(&tm);
    }

    /**
     * Given a time_t, find midnight to Monday that week, in the local
     * timezone.  Weeks start on Monday as far as I'm concerned.
     */
    time_t find_monday(time_t t)
    {
	// small steps: some days are shorter than 24 h, but not 20 h
	const unsigned step = 20 * 60 * 60;
	std::tm tm = *localtime(&t);
	while(tm.tm_wday != 1) {
	    t -= step;
	    tm = *localtime(&t);
	}
	tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
	return mktime(&tm);
    }

    /**
     * Given a time_t, find midnight to Monday next week, in the local
     * timezone.
     */
    time_t find_sunday(time_t t)
    {
	const unsigned step = 20 * 60 * 60;
	std::tm tm = *localtime(&t);
	while(tm.tm_wday == 1) {
	    t += step;
	    tm = *localtime(&t);
	}
	while(tm.tm_wday != 1) {
	    t += step;
	    tm = *localtime(&t);
	}
	tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
	return mktime(&tm);
    }

    /**
     * The local date "2018-10-06" for t.
     */
    std::string date(time_t t)
    {
	const std::tm tm = *localtime(&t);
	char buf[11];
	std::sprintf(buf, "%04d-%02d-%02d",
		     tm.tm_year + 1900,
		     tm.tm_mon + 1,
		     tm.tm_mday);
	return buf;
    }
}


Week::Week(std::time_t t)
    : begin {find_monday(t)},
      end {find_sunday(t)}
{
}

Week::Week(const std::string& ts)
    : Week(parse(ts))
{}

/**
 * Given an ISO timestamp, map it linearly onto the week so that the
 * beginning of Monday is 0 and the end of Sunday is 1.  Earlier times
 * map to <0; later to >1.
 *
 * So e.g. Tuesday 00:00 is 1/7 ~= 0.14.
 *
 * This gets a tiny bit unfair for weeks where there's a DST change,
 * and one of the days (in fact, the whole week) is longer or shorter
 * than usual.
 */
double Week::scale(const std::string& ts) const
{
    const std::time_t dt = parse(ts) - begin;
    return double(dt)/(end - begin);
}

Week Week::prev() const
{
    return Week {begin - 1};
}

std::string Week::monday() const
{
    return date(begin);
}

std::string Week::sunday() const
{
    return date(end - 1);
}

/**
 * Pretty-print the week for readability in my preferred
 * format (a traditional format in Sweden):
 *
 * 9-15.4.2018
 * 30.4-6.5.2018
 * 31.12.2018-6.1.2019
 */
std::ostream& Week::put(std::ostream& os) const
{
    const char dash[] = "\xe2\x80\x93";
    const std::tm mon = *localtime(&begin);
    const auto pend = end - 1;
    const std::tm sun = *localtime(&pend);

    if(mon.tm_mon == sun.tm_mon) {
	os << mon.tm_mday << dash
	   << sun.tm_mday << '.'
	   << sun.tm_mon + 1 << '.' << sun.tm_year + 1900;
    }
    else if(mon.tm_year == sun.tm_year) {
	os << mon.tm_mday << '.' << mon.tm_mon + 1 << dash
	   << sun.tm_mday << '.' << sun.tm_mon + 1 << '.'
	   << sun.tm_year + 1900;
    }
    else {
	os << mon.tm_mday << '.' << mon.tm_mon + 1 << '.'
	   << mon.tm_year + 1900
	   << dash
	   << sun.tm_mday << '.' << sun.tm_mon + 1 << '.'
	   << sun.tm_year + 1900;
    }
    return os;
}
