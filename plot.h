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
#ifndef WEATHER_PLOT_H
#define WEATHER_PLOT_H

#include "area.h"

#include "xml.h"
#include <iosfwd>

class Week;
class Files;


/**
 * Draw, in SVG, a plot like this: a main area of certain dimensions,
 * with the x axis being a week and the y axis a temperature scale
 * with 0° Celsius marked. Below it, an extra wind force area.
 *
 *  +---------+---------+---------+---------+---------+---------+---------+
 *  |         |         |         |         |         |         |         |
 *  |         |         |         |         |         |         |         |
 *  |         |         |         |         |         |         |         |
 *  |         |         |         |         |         |         |         |
 *  |         |         |         |         |         |         |         |
 *  |. . . . .|. . . . .|. . . . .|. . . . .|. . . . .|. . . . .|. . . . .|
 *  |         |         |         |         |         |         |         |
 *  |         |         |         |         |         |         |         |
 *  |         |         |         |         |         |         |         |
 *  +---------+---------+---------+---------+---------+---------+---------+
 *  |         |         |         |         |         |         |         |
 *  |         |         |         |         |         |         |         |
 *  +---------+---------+---------+---------+---------+---------+---------+
 */
class WeekPlot {
public:
    WeekPlot(std::ostream& os,
	     const Week& week,
	     const Area& temp,
	     const Area& rain,
	     const Area& wind);
    ~WeekPlot();

    void plot(const Week& week, bool use_wind_direction,
	      Files& files);

private:
    xml::ostream xos;
    Area temp;
    Area rain;
    Area wind;
};

#endif
