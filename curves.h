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
#ifndef WEATHER_CURVES_H
#define WEATHER_CURVES_H

#include "value.h"

#include <iosfwd>
#include <vector>

class Files;
class Week;

/**
 * 0--more curves for a specific week, read from a set of weather(5)
 * files.  They're several in two ways: there can be both a
 * temperature curve and wind curves, and the same week may appear
 * several times.
 */
class Curves {
public:
    Curves(const Week& week, Files& files, std::ostream& err);

    struct Sample {
	explicit Sample(double t) : t{t} {};
	double t;
	Value temperature_air;
	Value rain_amount;
	Value wind_force;
	Value wind_force_max;
	Value wind_direction;
	bool operator< (const Sample& other) const { return t < other.t; }

	bool empty() const { return !temperature_air &&
				    !rain_amount &&
				    !wind_direction &&
				    !wind_force &&
				    !wind_force_max; }
    };

    using Selection = decltype(&Sample::wind_force);
    using Curve = std::vector<Sample>;

    std::vector<Curve>::const_iterator begin() const { return val.begin(); }
    std::vector<Curve>::const_iterator end() const { return val.end(); }

private:
    std::vector<Curve> val;
};

#endif
