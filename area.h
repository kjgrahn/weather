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
#ifndef WEATHER_AREA_H
#define WEATHER_AREA_H

#include "value.h"


struct Dimensions {
    unsigned width;
    unsigned height;
};

struct Scale {
    int min;
    int max;

    bool in(int n) const { return min <= n && n <= max; }
};

/**
 * A plot area in a Plot: a rectangle covering a week on the x axis,
 * and a scale min/max on the y axis.  Responsible for the translation
 * from (time, temperature) to Plot coordinates.
 *
 * Always covers the full Plot width; areas may only be stacked on top
 * of each other.
 *
 *  :                                                :
 *  +------+------+------+------+------+------+------+
 *  |      :      :      :      :      :      :      |
 *  |      :      :      :      :      :      :      |
 *  |      :      :      :      :      :      :      |
 *  |      :      :      :      :      :      :      |
 *  |      :      :      :      :      :      :      |
 *  +------+------+------+------+------+------+------+
 *  :                                                :
 *
 */
class Area {
public:
    Area(const Scale& scale, const Dimensions& dim);
    Area(const Area& above, const Scale& scale, unsigned height);

    double xscale(double val) const;
    double yscale(double val) const;
    double yscale(Value val) const { return yscale(val.value()); }

    const unsigned offset;
    const Dimensions dim;
    const Scale scale;
};

#endif
