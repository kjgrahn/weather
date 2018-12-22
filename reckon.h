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
#ifndef WEATHER_RECKON_H
#define WEATHER_RECKON_H

#include "value.h"

#include <iostream>

/**
 * Some kind of dead reckoning: given a sequence of numbers a, b, c,
 * ...  emit a, b-a, c-b, ... in low resolution, but do so without
 * accumulating errors.
 *
 * This exists because of the SVG <path> element.  Relative movements
 * take less space, but if you calculate as doubles and then just
 * round each delta, rounding errors accumulate and you can notice a
 * shift in the painted curve.  At least when the movements are not
 * random.
 *
 * Uses class Value which is fixed-point with a single decimal, so
 * that limits the domain to where a single decimal makes sense.
 *
 * Also assumes repeatedly adding and subtracting doubles doesn't
 * introduce significant errors.
 */
class Reckon {
public:
    Value start(double n);
    Value moveto(double n);

private:
    double paintend;
};


class PathFormat {
public:
    PathFormat(std::ostream& os,
	       const std::pair<double, double>& xy);
    void add(const std::pair<double, double>& xy);

private:
    std::ostream& os;
    char cmd = 'M';
    unsigned short n = 0;
    Reckon rx;
    Reckon ry;
};


/**
 * Given a sequence of (x, y) coordinates, apply class Reckon to
 * generate a SVG <path d=...> attribute string with relative
 * movements.
 *
 * Undefined results if the sequence is empty.
 */
template <class Iter>
std::ostream& reckon(std::ostream& os, Iter begin, const Iter end)
{
    PathFormat pf(os, *begin++);

    while(begin != end) {
	pf.add(*begin++);
    }

    return os;
}

#endif
