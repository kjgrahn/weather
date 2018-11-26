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
#include "plot.h"

#include "curves.h"
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
    xml::attr attr(const char* name, int val) { return {name, std::to_string(val)}; }
    xml::attr attr(const char* name, unsigned val) { return {name, std::to_string(val)}; }
    xml::attr attr(const char* name, double val) { return {name, str(val)}; }

    xml::attr line(double x0, double y0, double x1, double y1)
    {
	std::string s = str(x0) + ',' + str(y0) + ' ' + str(x1) + ',' + str(y1);
	return {"points", s};
    }

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
		<< r.dim.width << ' ' << r.dim.height;
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
    }

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

    struct border {
	const Area a;
	const Area b;
    };

    xml::ostream& operator<< (xml::ostream& xos, const border& val)
    {
	xos << xml::elem("rect")
	    << attr("x", 0)
	    << attr("y", val.a.offset)
	    << attr("width",  val.a.dim.width)
	    << attr("height", val.a.dim.height + val.b.dim.height)
	    << attr("fill", "none")
	    << attr("stroke", "black")
	    << attr("stroke-width", 1)
	    << xml::end;
	return xos;
    }

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
	return xos;
    }
}


WeekPlot::WeekPlot(std::ostream& os,
		   const Area& temp,
		   const Area& wind)
    : xos{os},
      temp{temp},
      wind{wind}
{
    xos << xml::elem("svg")
	<< xml::attr("xmlns", "http://www.w3.org/2000/svg")
	<< viewbox(rect::total(temp, wind))
	<< xml::attr("version", "1.1");

    xos << rect::total(temp, wind)
	<< rect::thaw(temp)
	<< rect::freeze(temp)
	<< days{temp, wind}
	<< temperature{temp};
}

WeekPlot::~WeekPlot()
{
    xos << border{temp, wind}
	<< xml::end;
}

namespace {

    xml::attr line(const std::vector<std::pair<double, double>>& v)
    {
	std::string s;
	for(const auto& p : v) {
	    s += str(p.first) + ',' + str(p.second) + ' ';
	}
	if(s.size()) s.pop_back();
	return {"points", s};
    }

    void line(xml::ostream& xos, const Area& area,
	      const Curves::Curve& curve,
	      Curves::Selection selection,
	      const char* color = "black")
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

	if(!flatline) {
	    xos << xml::elem("polyline")
		<< attr("stroke", color) << attr("stroke-width", "1")
		<< attr("fill", "none")
		<< line(s)
		<< xml::end;
	}
    }
}

/**
 * If the files (in weather(5) format) contains data on temperature
 * and/or wind for the particular week, plot it.
 */
void WeekPlot::plot(const Week& week, Files& files)
{
    for(const auto& curve: Curves{week, files, std::cerr}) {
	line(xos, temp, curve, &Curves::Sample::temperature_air);
	line(xos, wind, curve, &Curves::Sample::wind_force_max, "#808080");
	line(xos, wind, curve, &Curves::Sample::wind_force);
    }
}
