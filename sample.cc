/*
 * Copyright (c) 2018, 2019 Jörgen Grahn
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
     * Translate from Trafikverket's "Precipitation.Type"
     * to none/rain/snow/other.
     */
    std::string translate(const std::string& ptype)
    {
	static const std::unordered_map<std::string, std::string> map {
	    {"Duggregn",		"rain"},
	    {"Hagel",			"other"},
	    {"Ingen nederb\303\266rd",	"none"},
	    {"Regn",			"rain"},
	    {"Sn\303\266",		"snow"},
	    {"Sn\303\266blandat regn",	"other"},
	    {"Underkylt regn",		"other"},
	};
	auto it = map.find(ptype);
	if(it != end(map)) return it->second;
	return "other";
    }

    /**
     * Parse the /Response/Result XML document to a map
     * Station -> vector of Samples.
     *
     * /Response/Result contains 0..N <WeatherStation>s, each with a
     * distinct <Id> and 0..N <Measurement> and <MeasurementHistory>
     * elements, which are the samples.
     *
     * Multiple samples with the same timestamp are discarded. Parse
     * errors cause an empty map, or a map with absent stations, to be
     * returned.
     */
    std::unordered_map<std::string,
		       Samples> parse(xml::Doc* doc)
    {
	xmlSetGenericErrorFunc(nullptr, nop);

	std::unordered_map<std::string, Samples> acc;

	xml::xpath::Ctx* ctx = xmlXPathNewContext(doc);

	xml::xpath::Obj id_match {ctx, "/RESPONSE/RESULT/WeatherStation/Id"};
	for(xml::Node* id_node : id_match) {

	    std::unordered_set<std::string> times;
	    auto seen = [&times] (const std::string& t) {
			    auto it = times.find(t);
			    if(it!=end(times)) return true;
			    times.insert(t);
			    return false;
			};

	    const std::string station = content(id_node);

	    xml::xpath::Obj meas_match {ctx, id_node->parent, "./*[MeasureTime]"};
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

		sample.time = get("MeasureTime");
		if(seen(sample.time)) continue;

		set("temperature.road", "Road/Temp");
		set("temperature.air",  "Air/Temp");
		set("humidity",         "Air/RelativeHumidity");
		set("wind.direction",   "Wind/Direction");
		set("wind.force",       "Wind/Force");
		set("wind.force.max",   "Wind/ForceMax");

		set("rain.amount",      "Precipitation/Amount");
		std::string ptype = get("Precipitation/Type");
		if(ptype.size()) {
		    sample.data["rain.type"] = translate(ptype);
		}

		acc[station].push_back(sample);
	    }
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
