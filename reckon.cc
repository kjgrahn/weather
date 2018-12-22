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
#include "reckon.h"

Value Reckon::start(double n)
{
    Value paint{n};
    paintend = paint.value();
    return paint;
}

Value Reckon::moveto(double n)
{
    Value delta{n - paintend};
    paintend += delta.value();
    return delta;
}

PathFormat::PathFormat(std::ostream& os,
		       const std::pair<double, double>& xy)
    : os{os}
{
    const double x = xy.first;
    const double y = xy.second;
    os << cmd << rx.start(x) << ' ' << ry.start(y) << ' ';
    cmd = 'l';
}

void PathFormat::add(const std::pair<double, double>& xy)
{
    n++;
    if(n == 10) {
	cmd = '\n';
	n = 0;
    }
    const double x = xy.first;
    const double y = xy.second;
    os << cmd << rx.moveto(x) << ' ' << ry.moveto(y);
    cmd = ' ';
}
