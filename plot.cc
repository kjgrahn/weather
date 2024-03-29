/*
 * Copyright (c) 2018 J�rgen Grahn
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
#include "plot.h"

#include "week.h"
#include "curves.h"
#include "direction.h"
#include "path.h"
#include "files...h"

namespace {

    /**
     * A floating-point number formatted with one decimal (because
     * that's kind of a reasonable scale if the full plot is around
     * 1000 wide).
     */
    std::string str(double val)
    {
	char buf[20];
	std::snprintf(buf, sizeof buf, "%.1f", val);
	return buf;
    }

    xml::attr attr(const char* name, const char* val) { return {name, val}; }
    xml::attr attr(const char* name, const std::string& val) { return {name, val}; }
    xml::attr attr(const char* name, int val) { return {name, std::to_string(val)}; }
    xml::attr attr(const char* name, unsigned val) { return {name, std::to_string(val)}; }
    xml::attr attr(const char* name, double val) { return {name, str(val)}; }

    xml::attr line(double x0, double y0, double x1, double y1)
    {
	std::string s = str(x0) + ',' + str(y0) + ' ' + str(x1) + ',' + str(y1);
	return {"points", s};
    }

    /**
     * Filled rectangles.
     */
    namespace rect {

	struct Rect {
	    const unsigned x;
	    const unsigned y;
	    const Dimensions dim;
	    const char* const fill;
	};

	xml::ostream& operator<< (xml::ostream& xos, const Rect& val)
	{
	    return xos << xml::elem("rect")
		       << attr("x", val.x)
		       << attr("y", val.y)
		       << attr("width",  val.dim.width)
		       << attr("height", val.dim.height)
		       << attr("fill", val.fill)
		       << xml::end;
	}

	xml::attr viewbox(const Rect& r)
	{
	    std::ostringstream oss;
	    oss << r.x << ' ' << r.y << ' '
		<< r.dim.width + 40 << ' ' << r.dim.height;
	    return {"viewBox", oss.str()};
	}

	Rect total(const Area& a, const Area& b)
	{
	    return {0, a.offset,
		    {a.dim.width, a.dim.height + b.dim.height},
		    "#e0e0e0"};
	}

	Rect freeze(const Area& a)
	{
	    unsigned z = a.yscale(0);
	    return {0, z,
		    {a.dim.width, a.dim.height - z},
		    "#c0d0f0"};
	}

	Rect thaw(const Area& a)
	{
	    unsigned z = a.yscale(0);
	    return {0, a.offset,
		    {a.dim.width, z - a.offset},
		    "#e0d0b0"};
	}

	Rect summer(const Area& a)
	{
	    unsigned z = a.yscale(20);
	    return {0, a.offset,
		    {a.dim.width, z},
		    "#ddb888"};
	}
    }

    /**
     * Borders between weekdays, and shaded Saturday--Sunday.
     */
    struct days {
	const Area a;
	const Area b;
    };

    xml::ostream& operator<< (xml::ostream& xos, const days& val)
    {
	double friday = val.a.xscale(5 / 7.0);
	xos << xml::elem("rect")
	    << attr("x", friday)
	    << attr("y", val.a.offset)
	    << attr("width",  val.a.dim.width - friday)
	    << attr("height", val.a.dim.height + val.b.dim.height)
	    << attr("fill", "#f00000")
	    << attr("opacity", ".1")
	    << xml::end;

	for(unsigned n=1; n<7; n++) {
	    double x = val.a.xscale(n / 7.0);

	    xos << xml::elem("polyline")
		<< attr("stroke", "#808080") << attr("stroke-width", ".5")
		<< attr("fill", "none")
		<< line(x, 0, x, val.a.dim.height + val.b.dim.height)
		<< xml::end;
	}
	return xos;
    }

    /**
     * A single black border around some areas.
     */
    struct border {
	explicit border(const Area& a)
	    : offset{a.offset},
	      dim{a.dim}
	{}
	border(const Area& a, const Area& b)
	    : offset{a.offset},
	      dim{a.dim.width, a.dim.height + b.dim.height}
	{}
	const unsigned offset;
	const Dimensions dim;
    };

    xml::ostream& operator<< (xml::ostream& xos, const border& val)
    {
	xos << xml::elem("rect")
	    << attr("x", 0)
	    << attr("y", val.offset)
	    << attr("width",  val.dim.width)
	    << attr("height", val.dim.height)
	    << attr("fill", "none")
	    << attr("stroke", "#808080")
	    << attr("stroke-width", ".5")
	    << xml::end;
	return xos;
    }

    /**
     * Temperature scale in an area: -10, 0, +10, ...
     */
    struct temperature {
	const Area a;
    };

    xml::ostream& operator<< (xml::ostream& xos, const temperature& val)
    {
	auto& a = val.a;

	for(int t = -40; t < 50; t+= 10) {
	    if(!a.scale.in(t)) continue;
	    double y = a.yscale(t);

	    xos << xml::elem("polyline")
		<< attr("stroke", "#808080") << attr("stroke-width", ".5")
		<< attr("fill", "none")
		<< line(0, y, a.dim.width, y)
		<< xml::end;
	}

	xos << xml::elem("text")
	    << attr("font-family", "sans-serif")
	    << attr("font-size", 12)
	    << attr("fill", "black");

	auto tick = [&xos] (unsigned y, const std::string& s) {
	    const char deg[] = "\xc2\xb0";
	    xos << xml::elem("tspan")
	    << attr("x", 700)
	    << attr("y", y)
	    << s + deg
	    << xml::end;
	};
	tick( 44, "+20");
	tick( 84, "+10");
	tick(124, "\xc2\xa0\xc2\xa0" "0");
	tick(164, "\xe2\x80\x93" "10");
	tick(204, "\xe2\x80\x93" "20");
	xos << xml::end;

	return xos;
    }

    /**
     * The week rendered as text in an area.
     */
    struct daterange {
	Week week;
	Area a;
    };

    xml::ostream& operator<< (xml::ostream& xos, const daterange& val)
    {
	xos << xml::elem("text")
	    << attr("font-family", "serif")
	    << attr("font-weight", "bold")
	    << attr("font-style", "italic")
	    << attr("opacity", ".1")
	    << attr("font-size", "60")
	    << attr("text-anchor", "middle")
	    << attr("x", val.a.dim.width/2)
	    << attr("y", val.a.dim.height/2 + 60/2)
	    << val.week
	    << xml::end;
	return xos;
    }
}


WeekPlot::WeekPlot(std::ostream& os,
		   const Week& week,
		   const Area& temp,
		   const Area& rain,
		   const Area& wind)
    : xos{os},
      temp{temp},
      rain{rain},
      wind{wind}
{
    xos << xml::elem("svg")
	<< xml::attr("xmlns", "http://www.w3.org/2000/svg")
	<< viewbox(rect::total(temp, wind))
	<< xml::attr("version", "1.1");

    xos << rect::total(temp, wind)
	<< rect::thaw(temp)
	<< rect::freeze(temp)
	<< rect::summer(temp)
	<< daterange{week, temp}
	<< days{temp, wind}
	<< temperature{temp};
}

WeekPlot::~WeekPlot()
{
    xos << border{temp}
	<< border{wind}
	<< border{temp, wind}
	<< xml::end;
}

namespace {

    /**
     * Extract a certain selection from a curve and translate
     * to plot coordinates.  If all samples are empty, return
     * an empty curve rather than a flat line.
     */
    std::vector<std::pair<double, double>>
    translate(const Area& area,
	      const Curves::Curve& curve,
	      Curves::Selection selection)
    {
	std::vector<std::pair<double, double>> s;
	bool flatline = true;

	for(const auto& sample : curve) {
	    double x = area.xscale(sample.t);
	    const Value val = sample.*selection;
	    double y = area.yscale(val);

	    s.push_back({x, y});
	    if(val!=0) flatline = false;
	}

	if(flatline) s.clear();
	return s;
    }

    /**
     * Render a curve as a colored line, possibly with holes.
     */
    void line(xml::ostream& xos, const Area& area,
	      const Curves::Curve& curve,
	      Curves::Selection selection,
	      const char* color = "black")
    {
	const auto s = translate(area, curve, selection);
	if(s.empty()) return;

	const double hour = area.xscale(1.0/7/24);

	xos << xml::elem("path")
	    << attr("stroke", color)
	    << attr("stroke-width", "1")
	    << attr("stroke-linejoin", "round")
	    << attr("fill", "none")
	    << attr("d", path::line(hour, s))
	    << xml::end;
    }

    /**
     * Render a curve as a semi-transparent blue area above the x
     * axis, possibly with holes.
     */
    void water(xml::ostream& xos, const Area& area,
	      const Curves::Curve& curve,
	      Curves::Selection selection)
    {
	const auto s = translate(area, curve, selection);
	if(s.empty()) return;

	const double nil = area.yscale(0);

	xos << xml::elem("path")
	    << attr("fill", "#4060c0")
	    << attr("opacity", ".5")
	    << attr("d", path::fill(nil, s))
	    << xml::end;
    }
}

/**
 * If the files (in weather(5) format) contains data for the
 * particular week, plot it.
 */
void WeekPlot::plot(const Week& week, bool use_wind_direction,
		    Files& files)
{
    const Curves curves{week, files, std::cerr};

    for(auto& curve: curves) water(xos, rain, curve, &Curves::Sample::rain_amount);
    for(auto& curve: curves) line(xos, temp, curve, &Curves::Sample::temperature_air);

    for(auto& curve: curves) line(xos, wind, curve, &Curves::Sample::wind_force_max, "#a0a0c0");
    for(auto& curve: curves) if(use_wind_direction && direction(xos, wind, curve)) break;
    for(auto& curve: curves) line(xos, wind, curve, &Curves::Sample::wind_force);
}
