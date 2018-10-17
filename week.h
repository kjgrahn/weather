/* -*- c++ -*-
 *
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
#ifndef WEATHER_WEEK_H
#define WEATHER_WEEK_H

#include <ctime>
#include <string>
#include <iosfwd>


/**
 * A week in local time, and with Monday as the first day of week.
 * Specified by a time point in the week.
 *
 * You can ask if a time is inside the week, and find out how far into
 * the week it is.  You can also find the preceding week.
 */
class Week {
public:
    explicit Week(std::time_t t);
    explicit Week(const std::string& ts);

    bool operator== (const Week& other) const {
	return begin == other.begin;
    }
    bool operator!= (const Week& other) const {
	return !(*this == other);
    }

    double scale(const std::string& ts) const;

    Week prev() const;
    std::string monday() const;
    std::string sunday() const;
    std::ostream& put(std::ostream& os) const;

private:
    time_t begin;
    time_t end;
};

inline
std::ostream& operator<< (std::ostream& os, const Week& val)
{
    return val.put(os);
}

#endif
