/*
 * Copyright (c) 2018, 2019, 2020 Jörgen Grahn
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
#include "sample.h"

#include <unordered_set>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <libxml/parser.h>
#include <libxml/xpath.h>


namespace {

    const char* cast(const xmlChar* s)
    {
	return reinterpret_cast<const char*>(s);
    }

    const xmlChar* cast(const char* s)
    {
	return reinterpret_cast<const xmlChar*>(s);
    }

    namespace xml {
	using Doc = xmlDoc;
	using Node = xmlNode;

	namespace xpath {
	    using Ctx = xmlXPathContext;

	    // wrapper for xmlXPathObject
	    class Obj {
	    public:
		Obj(Ctx* ctx, const char* expr)
		    : val(xmlXPathEvalExpression(cast(expr), ctx))
		{}
		Obj(Ctx* ctx, Node* root, const char* expr)
		    : val(xmlXPathNodeEval(root, cast(expr), ctx))
		{}
		~Obj()
		{
		    xmlXPathFreeObject(val);
		}

		Node** begin() const { return val->nodesetval->nodeTab; }
		Node** end() const { return begin() + val->nodesetval->nodeNr; }

	    private:
		xmlXPathObject* val;
	    };
	}
    }

    std::vector<xml::Node*> nodes(xml::xpath::Obj& obj)
    {
	return {obj.begin(), obj.end()};
    }

    const std::string content(xml::Node* node)
    {
	auto p = xmlNodeGetContent(node);
	std::string s = cast(p);
	xmlFree(p);
	return s;
    }

    void nop(void*, const char*, ...) {}

    /**
     * Multiplication, but with strings and "" instead of zero. For
     * translating from mm rain in 10 minutes to mm/h.
     */
    std::string multiply(const std::string& s, unsigned k)
    {
	const double n = std::strtod(s.c_str(), nullptr);
	char buf[10];
	std::snprintf(buf, sizeof buf, "%.1f", n * k);
	const std::string res = buf;
	if (res=="0.0") return "";
	return res;
    }

    /**
     * Translate from Trafikverket's rain/snow booleans
     * to none/rain/snow/other.
     */
    std::string translate(const std::string& rain, const std::string& snow)
    {
	const unsigned rs = 10 * (rain=="true") + (snow=="true");
	switch (rs) {
	case  0: return "none";
	case  1: return "snow";
	case 10: return "rain";
	case 11: return "other";
	}
    }

    /**
     * Parse the /Response/Result XML document containing
     * <WeatherObservation>s from various stations, to a map
     * Station -> vector of Samples.
     *
     * Parse errors cause an empty map, or a map with absent stations,
     * to be returned.
     */
    std::unordered_map<std::string,
		       Samples> parse(xml::Doc* doc)
    {
	xmlSetGenericErrorFunc(nullptr, nop);

	std::unordered_map<std::string, Samples> acc;

	xml::xpath::Ctx* ctx = xmlXPathNewContext(doc);

	xml::xpath::Obj meas_match {ctx, "/RESPONSE/RESULT/WeatherObservation"};
	for(auto measurement: meas_match) {
	    Sample sample;

	    auto get = [ctx, measurement] (const char* expr) -> std::string {
			   xml::xpath::Obj match {ctx, measurement, expr};
			   auto nn = nodes(match);
			   if (nn.size() != 1) return "";
			   return content(nn.front());
		       };
	    auto set = [get, &sample] (const char* name,
				       const char* expr) {
			   std::string val = get(expr);
			   if (val.size()) sample.data[name] = val;
		       };

	    const std::string station = get("Measurepoint/Id");

	    sample.time = get("Sample");

	    set("temperature.road", "Surface/Temperature/Value");
	    set("temperature.air",  "Air/Temperature/Value");
	    set("humidity",         "Air/RelativeHumidity/Value");
	    set("wind.direction",   "Wind/Direction/Value");
	    set("wind.force",       "Wind/Speed/Value");
	    set("wind.force.max",   "Aggregated30minutes/Wind/SpeedMax/Value");

	    const auto water = get("Aggregated10minutes/Precipitation/TotalWaterEquivalent/Value");
	    const auto mm = multiply(water, 6);
	    if (mm.size()) sample.data["rain.amount"] = mm;

	    const auto rt = translate(get("Aggregated10minutes/Precipitation/Rain"),
				      get("Aggregated10minutes/Precipitation/Snow"));
	    if (rt!="none") sample.data["rain.type"] = rt;

	    acc[station].push_back(sample);
	}

	xmlXPathFreeContext(ctx);
	return acc;
    }
}


std::unordered_map<std::string,
		   Samples> parse(const std::string& buf)
{
    xml::Doc* doc = xmlParseMemory(buf.data(), buf.size());
    if(!doc) return {};
    const auto acc = parse(doc);
    xmlFreeDoc(doc);

    return acc;
}
