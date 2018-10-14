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
#include "sample.h"

#include <unordered_set>
#include <iostream>
#include <algorithm>

#include <libxml/parser.h>
#include <libxml/xpath.h>


namespace {

    namespace xml {
	using Doc = xmlDoc;
	using Node = xmlNode;

	namespace xpath {
	    using Ctx = xmlXPathContext;
	    using Obj = xmlXPathObject;
	}
    }

    const char* cast(const xmlChar* s)
    {
	return reinterpret_cast<const char*>(s);
    }

    const xmlChar* cast(const char* s)
    {
	return reinterpret_cast<const xmlChar*>(s);
    }

    xml::xpath::Obj* eval(xml::xpath::Ctx* ctx,
			  const char* expr)
    {
	return xmlXPathEvalExpression(cast(expr), ctx);
    }

    xml::xpath::Obj* eval(xml::xpath::Ctx* ctx,
			  xml::Node* root,
			  const char* expr)
    {
	return xmlXPathNodeEval(root, cast(expr), ctx);
    }

    std::vector<xml::Node*> nodes(xml::xpath::Obj* match)
    {
	auto& nsv = *match->nodesetval;
	return {nsv.nodeTab, nsv.nodeTab + nsv.nodeNr};
    }

    std::vector<xml::Node*> nodes(const xml::Node& node)
    {
	std::vector<xml::Node*> acc;
	for(auto p = node.children; p!=node.last; p = p->next) {
	    acc.push_back(p);
	}
	return acc;
    }

    /**
     * Parse the /Response/Result/WeatherStation XML document to a
     * vector of Samples.  Multiple samples with the same timestamp
     * are discarded. Parse errors cause an empty vector to be
     * returned.
     */
    std::vector<Sample> parse(xml::Doc* doc)
    {
	std::vector<Sample> acc;

	xml::xpath::Ctx* ctx = xmlXPathNewContext(doc);

	std::unordered_set<std::string> times;
	auto seen = [&times] (const std::string& t) {
	    auto it = times.find(t);
	    if(it!=end(times)) return true;
	    times.insert(t);
	    return false;
	};

	xml::xpath::Obj* const match = eval(ctx, "/RESPONSE/RESULT/WeatherStation/*[MeasureTime]");
	for(xml::Node* measurement : nodes(match)) {

	    Sample sample;

	    auto get = [ctx, measurement] (const char* expr) {
		auto match = eval(ctx, measurement, expr);
		auto nn = nodes(match);
		if (nn.size() != 1) return "";
		return cast(xmlNodeGetContent(nn.front()));
	    };
	    auto set = [get, &sample] (const char* name,
				       const char* expr) {
		std::string val = get(expr);
		if (val.size()) sample.data[name] = val;
	    };

	    sample.time = get("MeasureTime");
	    if(seen(sample.time)) continue;

	    set("temperature.road", "Road/Temp");
	    set("temperature.air",  "Air/Temp");
	    set("humidity",         "Air/RelativeHumidity");
	    set("wind.direction",   "Wind/Direction");
	    set("wind.force",       "Wind/Force");
	    set("wind.force.max",   "Wind/ForceMax");

	    acc.push_back(sample);
	}

	xmlXPathFreeContext(ctx);
	return acc;
    }
}


std::vector<Sample> parse(const std::string& buf)
{
    xml::Doc* doc = xmlParseMemory(buf.data(), buf.size());
    const auto acc = parse(doc);
    xmlFreeDoc(doc);

    return acc;
}

/**
 * Like parse(std::string) but reads from a named file.
 */
std::vector<Sample> parse(const char* name)
{
    xml::Doc* doc = xmlParseFile(name);
    const auto acc = parse(doc);
    xmlFreeDoc(doc);

    return acc;
}


namespace {

    std::ostream& ljust(std::ostream& os, const std::string& s, size_t n)
    {
	os << s;
	if(s.size() < n) {
	    os << std::string(n - s.size(), ' ');
	}
	return os;
    }

    std::ostream& rjust(std::ostream& os, const std::string& s, size_t n)
    {
	if(s.size() < n) {
	    os << std::string(n - s.size(), ' ');
	}
	return os << s;
    }
}

std::ostream& operator<< (std::ostream& os, const Sample& val)
{
    os << "date: " << val.time << '\n';

    auto put = [&val] (std::ostream& os, const char* name) {
	const auto it = val.data.find(name);
	if(it==end(val.data)) return;

	ljust(os, it->first, 16) << ": ";
	rjust(os, it->second, 5) << '\n';
    };

    put(os, "temperature.road");
    put(os, "temperature.air");
    put(os, "humidity");
    put(os, "wind.direction");
    put(os, "wind.force");
    put(os, "wind.force.max");

    return os;
}

/**
 * Sort and print a sequence of samples to 'os', but first print
 * 'prefix'.
 */
void render(std::ostream& os, const char* prefix,
	    std::vector<Sample> samples)
{
    std::sort(begin(samples), end(samples));

    os << prefix;
    const char* delimiter = "";
    for(const auto& sample: samples) {
	os << delimiter << sample;
	delimiter = "\n";
    }
}
